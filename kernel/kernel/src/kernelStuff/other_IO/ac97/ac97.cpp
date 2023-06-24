#include "ac97.h"
#include "../../../OSDATA/osdata.h"

// https://github.com/byteduck/duckOS/blob/master/kernel/device/AC97Device.cpp

namespace AC97
{
    AC97Driver::AC97Driver(PCI::PCIDeviceHeader* pciBaseAddress)
    {
        PCIBaseAddress = pciBaseAddress;
        osData.debugTerminalWindow->Log("YOOO AC97 YES");


        uint64_t address = (uint64_t)PCIBaseAddress;
        //IRQHandler(PCI::read_byte(address, PCI_INTERRUPT_LINE)),
        //uint8_t irqId = PCI::read_byte(address, PCI_INTERRUPT_LINE);
        //Panic("AC97 IRQ: {}", to_string(irqId), true);
        
        // m_address = address;
        m_mixer_address = PCI::read_word(address, PCI_BAR0) & ~1;
        m_bus_address = PCI::read_word(address, PCI_BAR1) & ~1;
        m_output_channel = m_bus_address + BusRegisters::NABM_PCM_OUT,
        //m_output_buffer_region = MM.alloc_dma_region(0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        //m_output_buffer_descriptor_region = MM.alloc_dma_region(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        //m_output_buffer_region = (uint8_t*)_Malloc(0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        //m_output_buffer_descriptor_region = (uint8_t*)_Malloc(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        m_output_buffer_region = (uint8_t*)GlobalAllocator->RequestPages(AC97_OUTPUT_BUFFER_PAGES);
        //Panic("BRUH {}", ConvertHexToString((uint64_t)m_output_buffer_region), true);
        m_output_buffer_descriptor_region = (uint8_t*)GlobalAllocator->RequestPages(1);//(uint8_t*)_Malloc(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        m_output_buffer_descriptors = (BufferDescriptor*) m_output_buffer_descriptor_region;

        osData.ac97Driver = this;

        //Enable bus mastering and interrupts
        PCI::enable_interrupt(address);
        PCI::enable_bus_mastering(address);

        //Initialize the card with cold reset of bus and mixer, enable interrupts
        auto control = inb(m_bus_address + BusRegisters::GLOBAL_CONTROL);
        control |= GlobalControl::COLD_RESET | GlobalControl::INTERRUPT_ENABLE;
        outb(m_bus_address + BusRegisters::GLOBAL_CONTROL, control);
        write_mixer(RESET, 1);

        //TODO: Verify version?

        //Set master volume and reset pcm out channel
        write_mixer(MASTER_VOLUME, 0);
        write_mixer(PCM_VOLUME, 0);

        const int wantedSampleRate = 10000;

        set_sample_rate(wantedSampleRate);
        reset_output();

        //if (m_sample_rate != wantedSampleRate)
        //    Panic("AC97: Failed to set sample rate! GOT: {}", to_string((int)m_sample_rate), true);
    }


    void AC97Driver::handle_irq() 
    {
        //Read the status
        auto status_byte = inw(m_output_channel + ChannelRegisters::STATUS);
        BufferStatus status = {.value = status_byte};

        if(status.fifo_error)
            Panic("AC97 GOT FIFO ERROR!");//KLog::err("AC97", "Encountered FIFO error!");

        //If we're not done, don't do anything
        if(!status.completion_interrupt_status)
            return;

        status.value = 0;
        status.completion_interrupt_status = true;
        status.last_valid_interrupt = true;
        status.fifo_error = true;
        outw(m_output_channel + ChannelRegisters::STATUS, status.value);

        auto current_index = inb(m_output_channel + ChannelRegisters::CURRENT_INDEX);
        auto last_valid_index = inb(m_output_channel + ChannelRegisters::LAST_VALID_INDEX);
        if(last_valid_index == current_index) {
            reset_output();
        }
        m_blocker = true;
    }

    void AC97Driver::reset_output() 
    {
        //Send reset command and wait for it to be cleared
        outb(m_output_channel + ChannelRegisters::CONTROL, ControlFlags::RESET_REGISTERS);
        while(inb(m_output_channel + ChannelRegisters::CONTROL) & ControlFlags::RESET_REGISTERS)
            io_wait(50);
        m_output_dma_enabled = false;
        m_current_buffer_descriptor = 0;
    }

    void AC97Driver::set_sample_rate(uint32_t sample_rate) {
        outw(m_mixer_address + MixerRegisters::SAMPLE_RATE, sample_rate);
        io_wait(50);
        m_sample_rate = inw(m_mixer_address + MixerRegisters::SAMPLE_RATE);
    }


    uint64_t AC97Driver::writeBuffer(uint64_t offset, uint8_t* buffer, uint64_t count)
    {

        handle_irq();
        //Write buffer by buffer
        uint64_t n_written = 0;
        while(count > 0) 
        {
            //Wait until we have a free buffer to write to
            // do 
            // {
            //     //Read the status, current index, and last valid index
            //     //TaskManager::ScopedCritical critical;
            //     auto status_byte = inw(m_output_channel + ChannelRegisters::STATUS);
            //     BufferStatus status = {.value = status_byte};
            //     auto current_index = inb(m_output_channel + ChannelRegisters::CURRENT_INDEX);
            //     auto last_valid_index = inb(m_output_channel + ChannelRegisters::LAST_VALID_INDEX);
            //     auto num_buffers_left = last_valid_index >= current_index ? last_valid_index - current_index : AC97_NUM_BUFFER_DESCRIPTORS - (current_index - last_valid_index);
            //     if(!status.is_halted)
            //         num_buffers_left++;
            //     if(num_buffers_left < AC97_NUM_BUFFER_DESCRIPTORS)
            //         break;
            //     //critical.exit();
            //     m_blocker = false;
            //     //TaskManager::current_thread()->block(m_blocker);
            //     // if(m_blocker.was_interrupted())
            //     //     return 0;
            // } while(m_output_dma_enabled);

            //If the output DMA is not currently enabled, reset the PCM channel to be sure
            if(!m_output_dma_enabled)
                reset_output();

            reset_output();

            //Copy as much data as is applicable to the current output buffer
            uint32_t* output_buffer = (uint32_t*)(m_output_buffer_region + 0x1000 * m_current_output_buffer_page);
            size_t num_bytes = min(count, 0x1000);
            //buffer.read((uint8_t*) output_buffer, n_written, num_bytes); // buff offset count
            _memcpy((uint8_t*) buffer + n_written, output_buffer, num_bytes);
            count -= num_bytes;
            n_written += num_bytes;

            //Create the buffer descriptor
            auto* descriptor = &m_output_buffer_descriptors[m_current_buffer_descriptor];
            descriptor->data_addr = (uint32_t)(((uint64_t)m_output_buffer_region) + 0x1000 * m_current_output_buffer_page);//m_output_buffer_region->object()->physical_page(m_current_output_buffer_page).paddr();
            descriptor->num_samples = num_bytes / sizeof(uint16_t);
            descriptor->flags = {false, true};

            //Set the buffer descriptor list address and last valid index in the channel registers
            outl(m_output_channel + ChannelRegisters::BUFFER_LIST_ADDR, (uint32_t)(uint64_t)m_output_buffer_descriptor_region);//m_output_buffer_descriptor_region->object()->physical_page(0).paddr());
            outb(m_output_channel + ChannelRegisters::LAST_VALID_INDEX, m_current_buffer_descriptor);

            //If the output DMA is not enabled already, enable it
            if(!m_output_dma_enabled) {
                auto ctrl = inb(m_output_channel + ChannelRegisters::CONTROL);
                ctrl |= ControlFlags::PAUSE_BUS_MASTER | ControlFlags::ERROR_INTERRUPT | ControlFlags::COMPLETION_INTERRUPT;
                outb(m_output_channel + ChannelRegisters::CONTROL, ctrl);
                m_output_dma_enabled = true;
            }

            //Increment buffer page and buffer descriptor index
            m_current_output_buffer_page++;
            m_current_output_buffer_page %= AC97_OUTPUT_BUFFER_PAGES;
            m_current_buffer_descriptor++;
            m_current_buffer_descriptor %= AC97_NUM_BUFFER_DESCRIPTORS;
        }

        return n_written;
    }

}