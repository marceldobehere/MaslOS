#include "pci.h"
#include "../../paging/PageTableManager.h"
#include "../../osData/osData.h"
#include "../ahci/ahci.h"
#include "../ac97/ac97.h"
#include "../serial/serial.h"

namespace PCI
{
    void EnumeratePCI(ACPI::MCFGHeader* mcfg)
    {
        AddToStack();
        int entries = (mcfg->Header.Length - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
        RemoveFromStack();
        
        AddToStack();

        for (int t = 0; t < entries; t++)
        {
            ACPI::DeviceConfig* newDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + sizeof(ACPI::DeviceConfig) * t);
            for (uint64_t bus = newDeviceConfig->StartBus; bus < newDeviceConfig->EndBus; bus++)
                EnumerateBus(newDeviceConfig->BaseAddress, bus);
        }
        RemoveFromStack();
    }

    void EnumerateBus(uint64_t baseAddress, uint64_t bus)
    {
        AddToStack();
        uint64_t offset = bus << 20;

        uint64_t busAddress = baseAddress + offset;

        //GlobalPageTableManager.MapMemory((void*)busAddress, (void*)busAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)busAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        for (uint64_t device = 0; device < 32; device++)
        {
            EnumerateDevice(busAddress, device);
        }
        RemoveFromStack();
    }

    void EnumerateDevice(uint64_t busAddress, uint64_t device) // Slot
    {
        AddToStack();
        uint64_t offset = device << 15;

        uint64_t deviceAddress = busAddress + offset;

        //GlobalPageTableManager.MapMemory((void*)deviceAddress, (void*)deviceAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)deviceAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        for (uint64_t function = 0; function < 8; function++)
        {
            EnumerateFunction(deviceAddress, function);
        }
        RemoveFromStack();
    }

    void EnumerateFunction(uint64_t deviceAddress, uint64_t function)
    {
        AddToStack();
        uint64_t offset = function << 12;

        uint64_t functionAddress = deviceAddress + offset;

        if (functionAddress == 0x0000000000000000) {RemoveFromStack(); return;}

        //GlobalPageTableManager.MapMemory((void*)functionAddress, (void*)functionAddress);
        
        PCIDeviceHeader* pciDeviceHeader  = (PCIDeviceHeader*)functionAddress;

        if (pciDeviceHeader ->Device_ID == 0x0000) {RemoveFromStack(); return;}
        if (pciDeviceHeader ->Device_ID == 0xFFFF) {RemoveFromStack(); return;}

        BasicRenderer* renderer = osData.debugTerminalWindow->renderer;
        renderer->Print(" - ");

        {
            const char* vendorName = GetVendorName(pciDeviceHeader->Vendor_ID);
            if (vendorName != unknownString)
                renderer->Print(vendorName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Vendor_ID));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* deviceName = GetDeviceName(pciDeviceHeader->Vendor_ID, pciDeviceHeader->Device_ID);
            if (deviceName != unknownString)
                renderer->Print(deviceName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Device_ID));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* className = GetClassName(pciDeviceHeader->Class);
            if (className != unknownString)
                renderer->Print(className);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Class));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* subclassName = GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->SubClass);
            if (subclassName != unknownString)
                renderer->Print(subclassName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->SubClass));
                renderer->Print(">");
            }
            renderer->Print(" / ");
        }

        {
            const char* progIFName = GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->SubClass, pciDeviceHeader->Prog_IF);
            if (progIFName != unknownString)
                renderer->Print(progIFName);
            else
            {
                renderer->Print("<");
                renderer->Print(ConvertHexToString(pciDeviceHeader->Prog_IF));
                renderer->Print(">");
            }
            //renderer->Print(" / ");
        }
        renderer->Println();

        // osData.debugTerminalWindow->renderer->Println("> BARS:", Colors.yellow);
        // for (int i = 0; i < 6; i++)
        // {
        //     osData.debugTerminalWindow->renderer->Print(" - BAR {}: ", to_string(i), Colors.orange);
        //     osData.debugTerminalWindow->renderer->Println("{}", ConvertHexToString(*(((uint32_t*)&((PCI::PCIHeader0*)pciDeviceHeader)->BAR0) + i)), Colors.orange);
        //     io_wait(1000);
        // }


        if (pciDeviceHeader->Class == 0x04 && pciDeviceHeader->SubClass == 0x01 && pciDeviceHeader->Prog_IF == 0x00)
        {
            new AC97::AC97Driver(pciDeviceHeader);
        }
        else if (pciDeviceHeader->Vendor_ID == 0x8086 && pciDeviceHeader->Device_ID == 0x3A3E)
        {
            new AC97::AC97Driver(pciDeviceHeader);
        }
        else if (pciDeviceHeader->Vendor_ID == 0x8086 && pciDeviceHeader->Device_ID == 0x3A6E)
        {
            new AC97::AC97Driver(pciDeviceHeader);
        }
        else if (pciDeviceHeader->Vendor_ID == 0x1022 && pciDeviceHeader->Device_ID == 0x15E3)
        {
            new AC97::AC97Driver(pciDeviceHeader);
        }
        else if (pciDeviceHeader->Class == 0x01 && pciDeviceHeader->SubClass == 0x06 && pciDeviceHeader->Prog_IF == 0x01)
        {
            new AHCI::AHCIDriver(pciDeviceHeader);
        }
        else if (pciDeviceHeader->Class == 0x07 && pciDeviceHeader->SubClass == 0x00)
        {
            Serial::pciCard = (uint64_t)pciDeviceHeader;
            //Serial::SerialPort= 0x2F8;
            if (!Serial::Init())
                Serial::pciCard = 0;
        }

        RemoveFromStack();
    }




    IOAddress get_address(PCIDeviceHeader* hdr, uint8_t field)
    {
        uint64_t addr = (uint64_t)hdr;
        return get_address(addr, field);
    }

    IOAddress get_address(uint64_t addr, uint8_t field)
    {
        IOAddress address;
        address.attrs.field = field;
        address.attrs.function = (addr >> 12) & 0b111; // 3 bit
        address.attrs.slot = (addr >> 15) & 0b11111; // 5 bit
        address.attrs.bus = (addr >> 20) & 0b11111111; // 8 bit
        address.attrs.enable = true;
        //0x80000000 
        return address;
    }

	uint8_t io_read_byte(uint64_t address, uint8_t field) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		return inb(PCI_DATA_PORT + (field & 3));
	}

	uint16_t io_read_word(uint64_t address, uint8_t field){
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		return inw(PCI_DATA_PORT + (field & 2));
	}

	uint32_t io_read_dword(uint64_t address, uint8_t field) {
	    outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		return inl(PCI_DATA_PORT);
	}

	void io_write_byte(uint64_t address, uint8_t field, uint8_t value) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		outb(PCI_DATA_PORT + (field & 3), value);
	}

	void io_write_word(uint64_t address, uint8_t field, uint16_t value) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		outw(PCI_DATA_PORT + (field & 2), value);
	}

	void io_write_dword(uint64_t address, uint8_t field, uint32_t value) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		outl(PCI_DATA_PORT, value);
	}

	void enable_interrupt(uint64_t address) {
		Command comm = {.value = io_read_word(address, PCI_COMMAND)};
		comm.attrs.interrupt_disable = false;
		io_write_word(address, PCI_COMMAND, comm.value);
	}

	void disable_interrupt(uint64_t address) {
		Command comm = {.value = io_read_word(address, PCI_COMMAND)};
		comm.attrs.interrupt_disable = true;
		io_write_word(address, PCI_COMMAND, comm.value);
	}

	void enable_bus_mastering(uint64_t address) {
		Command comm = {.value = io_read_word(address, PCI_COMMAND)};
		comm.attrs.bus_master = true;
		io_write_word(address, PCI_COMMAND, comm.value);
	}

	void disable_bus_mastering(uint64_t address) {
		Command comm = {.value = io_read_word(address, PCI_COMMAND)};
		comm.attrs.bus_master = false;
		io_write_word(address, PCI_COMMAND, comm.value);
	}

    void enable_io_space(uint64_t address)
    {
        Command comm = {.value = io_read_word(address, PCI_COMMAND)};
        comm.attrs.io_space = true;
        io_write_word(address, PCI_COMMAND, comm.value);
    }
	void disable_io_space(uint64_t address)
    {
        Command comm = {.value = io_read_word(address, PCI_COMMAND)};
        comm.attrs.io_space = false;
        io_write_word(address, PCI_COMMAND, comm.value);
    }
    void enable_mem_space(uint64_t address)
    {
        Command comm = {.value = io_read_word(address, PCI_COMMAND)};
        comm.attrs.mem_space = true;
        io_write_word(address, PCI_COMMAND, comm.value);
    }
	void disable_mem_space(uint64_t address)
    {
        Command comm = {.value = io_read_word(address, PCI_COMMAND)};
        comm.attrs.mem_space = false;
        io_write_word(address, PCI_COMMAND, comm.value);
    }

    void pci_read_bar(uint32_t* mask, uint64_t addr, uint32_t offset)
    {
        uint32_t data = io_read_dword(addr, offset);
        io_write_dword(addr, offset, 0xffffffff);
        *mask = io_read_dword(addr, offset);
        io_write_dword(addr, offset, data);
    }

    PCI_BAR_TYPE pci_get_bar(uint32_t* bar0, int bar_num, uint64_t addr)
    {
        PCI_BAR_TYPE bar;
        bar.io_address = 0;
        bar.size = 0;
        bar.mem_address = 0;
        bar.type = NONE;

        uint32_t* bar_ptr = bar0 + bar_num;

        if (*bar_ptr == NULL) 
        {
            bar.type = NONE;
            return bar;
        }

        uint32_t mask;
        pci_read_bar(&mask, addr, bar_num * sizeof(uint32_t));

        if (*bar_ptr & 0x04) 
        { //64-bit mmio
            bar.type = MMIO64;

            uint32_t* bar_ptr_high = bar0 + bar_num + 4;
            uint32_t mask_high;
            pci_read_bar(&mask_high, addr, (bar_num * sizeof(uint32_t)) + 0x4);

            bar.mem_address = ((uint64_t) (*bar_ptr_high & ~0xf) << 32) | (*bar_ptr & ~0xf);
            bar.size = (((uint64_t) mask_high << 32) | (mask & ~0xf)) + 1;
        } 
        else if (*bar_ptr & 0x01) 
        { //IO
            bar.type = IO;

            bar.io_address = (uint16_t)(*bar_ptr & ~0x3);
            bar.size = (uint16_t)(~(mask & ~0x3) + 1);
        } 
        else 
        { //32-bit mmio
            bar.type = MMIO32;

            bar.mem_address = (uint64_t) *bar_ptr & ~0xf;
            bar.size = ~(mask & ~0xf) + 1;
        }
        

        return bar;
    }

    PCI_BAR_TYPE pci_get_bar(PCIHeader0* addr, int bar_num)
    {
        return pci_get_bar(&(addr->BAR0), bar_num, (uint64_t)addr);
    }

    uint8_t read_byte(uint64_t address, PCI_BAR_TYPE type, uint8_t field)
    {
        if (type.type == PCI_BAR_TYPE_ENUM::MMIO32 || type.type == PCI_BAR_TYPE_ENUM::MMIO64)
            return *(uint8_t*)(type.mem_address + field);
        else if (type.type == PCI_BAR_TYPE_ENUM::IO)
            return inb(type.io_address + field);
    }

	uint16_t read_word(uint64_t address, PCI_BAR_TYPE type, uint8_t field)
    {
        if (type.type == PCI_BAR_TYPE_ENUM::MMIO32 || type.type == PCI_BAR_TYPE_ENUM::MMIO64)
            return *(uint16_t*)(type.mem_address + field);
        else if (type.type == PCI_BAR_TYPE_ENUM::IO)
            return inw(type.io_address + field);
    }

	uint32_t read_dword(uint64_t address, PCI_BAR_TYPE type, uint8_t field)
    {
        if (type.type == PCI_BAR_TYPE_ENUM::MMIO32 || type.type == PCI_BAR_TYPE_ENUM::MMIO64)
            return *(uint32_t*)(type.mem_address + field);
        else if (type.type == PCI_BAR_TYPE_ENUM::IO)
            return inl(type.io_address + field);
    }

	void write_byte(uint64_t address, PCI_BAR_TYPE type, uint16_t field, uint8_t value)
    {
        if (type.type == PCI_BAR_TYPE_ENUM::MMIO32 || type.type == PCI_BAR_TYPE_ENUM::MMIO64)
            *(uint8_t*)(type.mem_address + field) = value;
        else if (type.type == PCI_BAR_TYPE_ENUM::IO)
            outb(type.io_address + field, value);
    }

	void write_word(uint64_t address, PCI_BAR_TYPE type, uint16_t field, uint16_t value)
    {
        if (type.type == PCI_BAR_TYPE_ENUM::MMIO32 || type.type == PCI_BAR_TYPE_ENUM::MMIO64)
            *(uint16_t*)(type.mem_address + field) = value;
        else if (type.type == PCI_BAR_TYPE_ENUM::IO)
            outw(type.io_address + field, value);
    }

	void write_dword(uint64_t address, PCI_BAR_TYPE type, uint16_t field, uint32_t value)
    {
        if (type.type == PCI_BAR_TYPE_ENUM::MMIO32 || type.type == PCI_BAR_TYPE_ENUM::MMIO64)
            *(uint32_t*)(type.mem_address + field) = value;
        else if (type.type == PCI_BAR_TYPE_ENUM::IO)
            outl(type.io_address + field, value);
    }
}