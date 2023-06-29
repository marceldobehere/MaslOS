#include "ac97.h"
#include "../../../OSDATA/osdata.h"
#include "../../../Rendering/testoDebug.h"
#include "../../../musicTest/sbTest.h"
#include "../../../kernelStuff/other_IO/serial/serial.h"


// https://github.com/byteduck/duckOS/blob/master/kernel/device/AC97Device.cpp

namespace AC97
{
    void AC97Driver::HandleIRQ(interrupt_frame* frame)
    {
        //Panic("OO YES", true);
        //Serial::Writeln("<AC97 IRQ>");
        // if (!handle_irq())
        // {
        //     //Serial::Writeln("</NVM>");
        //     //return;
        // }
        handle_irq();
        doCheck = true;

        //needManualRestart = CheckMusic();    
         

        // if (osData.ac97Driver != NULL)
        // {
        //     //AddToStack();
        //     int amt = 1000;
        //     int step = 1;
        //     int hz = ((PIT::TimeSinceBootMS() / 10) % 100) * 2 + 200;
        //     //hz = 200;
        //     // frequency gets doubled bc we are sending mono
        //     uint16_t* testBuff = (uint16_t*)_Malloc(amt*2);

        //     MusicBit16Test::FillArray(testBuff, 0, amt, hz);            
        
        //     //AddToStack();
        //     uint64_t tCount = 0;
        //     tCount = osData.ac97Driver->writeBuffer(0, (uint8_t*)testBuff, amt*2);
        //     //RemoveFromStack();
        //     //Serial::Writeln("X> Wrote {} bytes", to_string(tCount));


        //     //AddToStack();
        //     _Free(testBuff);
        //     //RemoveFromStack();
        // }

        //Serial::Writeln("</AC97 IRQ: {}>", to_string(needManualRestart));  
    }

    bool AC97Driver::CheckMusic()
    {
        //return true;
        int c = audioDestination->RequestBuffers();
        if (c > 0)
        {
            if (osData.ac97Driver != NULL)
            {
                uint64_t tCount = 0;
                //Serial::Writeln("> Writing {} bytes", to_string(audioDestination->buffer->byteCount));
                tCount = osData.ac97Driver->writeBuffer(0, 
                (uint8_t*)(audioDestination->buffer->data), 
                audioDestination->buffer->byteCount);
 
                if (tCount != audioDestination->buffer->byteCount)
                {
                    Panic("AC97Driver::HandleIRQ: tCount != audioDestination->buffer->byteCount", true);
                }

                audioDestination->buffer->ClearBuffer();
                audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
                //lastDone = tDone;
                //Panic("bruh: {}", to_string(c), true);
                //return true;
                return false;
            }
        }

        return true;

        // if (audioDestination != NULL)
        // {
        //     bool tDone = audioDestination->AllBuffersDone();
        //     if (tDone)
        //     {
        //         if (!lastDone && 
        //             osData.ac97Driver != NULL)
        //         {
        //             // Do Stuff
        //             uint64_t tCount = 0;
        //             tCount = osData.ac97Driver->writeBuffer(0, 
        //             (uint8_t*)(audioDestination->buffer->data), 
        //             audioDestination->buffer->byteCount);

        //             if (tCount != audioDestination->buffer->byteCount)
        //             {
        //                 Panic("AC97Driver::HandleIRQ: tCount != audioDestination->buffer->byteCount", true);
        //             }

        //             audioDestination->buffer->ClearBuffer();
        //             audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
        //             lastDone = tDone;
        //             return false;
        //         }
        //     }
        //     else
        //     {
        //         audioDestination->RequestBuffers();
        //     }
        //     lastDone = tDone;
        //     return true;
        // }
    }

    AC97Driver::AC97Driver(PCI::PCIDeviceHeader* pciBaseAddress)
    {
        PCIBaseAddress = pciBaseAddress;
        PrintMsgStartLayer("AC97Driver");

        PrintMsg("> AC97 Driver Init");
        osData.debugTerminalWindow->Log("YOOO AC97 YES");


        uint64_t address = (uint64_t)PCIBaseAddress;
        //PrintMsgCol("> PCI Address: {}", ConvertHexToString(address), Colors.yellow);
        //Println();
        //IRQHandler(PCI::read_byte(address, PCI_INTERRUPT_LINE)),
        irqId = PCI::read_byte(address, PCI_INTERRUPT_LINE);
        //Panic("AC97 IRQ: {}", to_string(irqId), true);
        {
            IRQHandlerCallbackFuncs[irqId] = (void*)&HandleIRQ;
            IRQHandlerCallbackHelpers[irqId] = (void*)this;
        }
        
        // m_address = address;
        m_mixer_address = PCI::read_word(address, PCI_BAR0) & ~1;
        PrintMsgCol("> Mixer Address: {}", ConvertHexToString(m_mixer_address), Colors.yellow);
        m_bus_address = PCI::read_word(address, PCI_BAR1) & ~1;
        PrintMsgCol("> Bus Address: {}", ConvertHexToString(m_bus_address), Colors.yellow);
        m_output_channel = m_bus_address + BusRegisters::NABM_PCM_OUT;
        PrintMsgCol("> Output Channel: {}", ConvertHexToString(m_output_channel), Colors.yellow);
        Println();

        //m_output_buffer_region = MM.alloc_dma_region(0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        //m_output_buffer_descriptor_region = MM.alloc_dma_region(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        //m_output_buffer_region = (uint8_t*)_Malloc(0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        //m_output_buffer_descriptor_region = (uint8_t*)_Malloc(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        m_output_buffer_region = (uint8_t*)GlobalAllocator->RequestPages(AC97_OUTPUT_BUFFER_PAGES);
        GlobalPageTableManager.MapMemories((void*)m_output_buffer_region, (void*)m_output_buffer_region, AC97_OUTPUT_BUFFER_PAGES, true);
        _memset(m_output_buffer_region, 0, 0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        PrintMsgCol("> Output Buffer Region: {}", ConvertHexToString((uint64_t)m_output_buffer_region), Colors.yellow);
        //Panic("BRUH {}", ConvertHexToString((uint64_t)m_output_buffer_region), true);
        m_output_buffer_descriptor_region = (uint8_t*)GlobalAllocator->RequestPages(1);//(uint8_t*)_Malloc(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);
        GlobalPageTableManager.MapMemories((void*)m_output_buffer_descriptor_region, (void*)m_output_buffer_descriptor_region, 1, true);
        _memset(m_output_buffer_descriptor_region, 0, sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);
        PrintMsgCol("> Output Buffer Descriptor Region: {}", ConvertHexToString((uint64_t)m_output_buffer_descriptor_region), Colors.yellow);
        Println();

        m_output_buffer_descriptors = (BufferDescriptor*) m_output_buffer_descriptor_region;

        //Enable bus mastering and interrupts
        PCI::enable_interrupt(address);
        PrintMsg("> Enabled PCI Interrupts");
        PCI::enable_bus_mastering(address);
        PrintMsg("> Enabled PCI Bus Mastering");
        Println();

        //Initialize the card with cold reset of bus and mixer, enable interrupts
        auto control = inl(m_bus_address + BusRegisters::GLOBAL_CONTROL);
        control |= GlobalControl::COLD_RESET | GlobalControl::INTERRUPT_ENABLE;
        control &= ~(0b11 << 22);
        control |=   (0b00 << 22);
        outl(m_bus_address + BusRegisters::GLOBAL_CONTROL, control);
        write_mixer(RESET, 1);
        PrintMsg("> Initialized Card");
        Println();

        //TODO: Verify version?

        //Set master volume and reset pcm out channel
        write_mixer(MASTER_VOLUME, 0);
        PrintMsg("> Set Master Volume");
        write_mixer(PCM_VOLUME, 0x0808);
        PrintMsg("> Set PCM Volume");
        reset_output();
        


        const int wantedSampleRate = 48000;

        PrintMsg("> Setting Sample Rate to {}", to_string(wantedSampleRate));
        int tries = 10;
        while (tries-- > 0)
        {
            set_sample_rate(wantedSampleRate);
            if (m_sample_rate == wantedSampleRate)
                break;
            PIT::Sleep(10);
        }
        PrintMsg("> Card Sample Rate: {}", to_string((int)m_sample_rate));

        if (m_sample_rate != wantedSampleRate)
            PrintMsg("> Sample rate is borked!");//Panic("AC97: Failed to set sample rate! GOT: {}", to_string((int)m_sample_rate), true);
        
        reset_output();
        PrintMsg("> Reset Output");
        
        audioDestination = new Audio::BasicAudioDestination(
            //Audio::AudioBuffer::Create16Bit48KHzStereoBuffer(DEF_SAMPLE_COUNT)
            Audio::AudioBuffer::Create16Bit48KHzStereoBuffer(DEF_SAMPLE_COUNT)
        );
        audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
        //lastDone = true;
        needManualRestart = true;
        doCheck = true;

        if (osData.ac97Driver == NULL)
        {
            osData.ac97Driver = this;
            osData.audioDestinations.add(audioDestination);
            PrintMsg("> Set as main AC97 Driver");
        }

        PrintMsgEndLayer("AC97Driver");
    }


    bool AC97Driver::handle_irq() 
    {
        //Read the status
        auto status_byte = inw(m_output_channel + ChannelRegisters::STATUS);
        BufferStatus status = {.value = status_byte};

        if(status.fifo_error)
            Panic("AC97 GOT FIFO ERROR!");//KLog::err("AC97", "Encountered FIFO error!");

        //If we're not done, don't do anything
        if(!status.completion_interrupt_status)
            return false;


        //osData.debugTerminalWindow->newPosition.x--;

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
        return true;
    }

    void AC97Driver::reset_output() 
    {
       // io_wait(1000);
        //Send reset command and wait for it to be cleared
        int timeOut = 200;
        do
        {
            outb(m_output_channel + ChannelRegisters::CONTROL, ControlFlags::RESET_REGISTERS);
            io_wait(100);
        } while((timeOut-- > 0) && inb(m_output_channel + ChannelRegisters::CONTROL) & ControlFlags::RESET_REGISTERS);

        m_output_dma_enabled = false;
        m_current_buffer_descriptor = 0;
    }

    void AC97Driver::set_sample_rate(uint32_t sample_rate) {
        io_wait(1000);
        outw(m_mixer_address + MixerRegisters::SAMPLE_RATE, sample_rate);
        io_wait(1000);
        m_sample_rate = inw(m_mixer_address + MixerRegisters::SAMPLE_RATE);
    }


    uint64_t AC97Driver::writeBuffer(uint64_t offset, uint8_t* buffer, uint64_t count)
    {
        buffer += offset;
        count -= offset;
        //handle_irq();
        //Write buffer by buffer
        uint64_t n_written = 0;
        while(count > 0) 
        {
            // //Wait until we have a free buffer to write to
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

            //reset_output();

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