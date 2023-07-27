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

        switch (pciDeviceHeader->Class)
        {
            case 0x01:
            {
                switch (pciDeviceHeader->SubClass)
                {
                    case 0x06:
                    {
                        switch (pciDeviceHeader->Prog_IF)
                        {
                            case 0x01:
                            {
                                new AHCI::AHCIDriver(pciDeviceHeader);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
            case 0x04: // AC97 CLASS
            {
                switch (pciDeviceHeader->SubClass)
                {
                    case 0x01: // AC97 SUBCLASS
                    {
                        new AC97::AC97Driver(pciDeviceHeader);
                        break;
                    }

                    // case 0x03: // MAYBE AC97 SUBCLASS FOR TESTING IG
                    // {
                    //     new AC97::AC97Driver(pciDeviceHeader);
                    //     break;
                    // }
                }
                break;
            }
            case 0x07: // Serial PCI Card
            {
                switch (pciDeviceHeader->SubClass)
                {
                    case 0x00: // AC97 SUBCLASS
                    {
                        Serial::pciCard = (uint64_t)pciDeviceHeader;
                        //Serial::SerialPort= 0x2F8;
                        if (Serial::Init())
                        {
                            //Panic("SERIAL CARD YES", true);
                        }
                        else
                        {
                            // Serial::SerialWorks = true;
                            // Serial::Soutb(4, 0x0F);
                            // Serial::Writeln("TEST");
                            Serial::pciCard = 0;
                        }
                        
                        break;
                    }
                }
                break;
            }
        }

        // check for intel integrated ac97
        if (pciDeviceHeader->Vendor_ID == 0x8086 && pciDeviceHeader->Device_ID == 0x3A3E)
        {
            //Panic("YOO WE GOT THE INTEL AC97", true);
            new AC97::AC97Driver(pciDeviceHeader);
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

	uint8_t read_byte(uint64_t address, uint8_t field) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		return inb(PCI_DATA_PORT + (field & 3));
	}

	uint16_t read_word(uint64_t address, uint8_t field){
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		return inw(PCI_DATA_PORT + (field & 2));
	}

	uint32_t read_dword(uint64_t address, uint8_t field) {
	    outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		return inl(PCI_DATA_PORT);
	}

	void write_byte(uint64_t address, uint8_t field, uint8_t value) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		outb(PCI_DATA_PORT + (field & 3), value);
	}

	void write_word(uint64_t address, uint8_t field, uint16_t value) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		outw(PCI_DATA_PORT + (field & 2), value);
	}

	void write_dword(uint64_t address, uint8_t field, uint32_t value) {
		outl(PCI_ADDRESS_PORT, get_address(address, field).value);
		outl(PCI_DATA_PORT, value);
	}

	void enable_interrupt(uint64_t address) {
		Command comm = {.value = read_word(address, PCI_COMMAND)};
		comm.attrs.interrupt_disable = false;
		write_word(address, PCI_COMMAND, comm.value);
	}

	void disable_interrupt(uint64_t address) {
		Command comm = {.value = read_word(address, PCI_COMMAND)};
		comm.attrs.interrupt_disable = true;
		write_word(address, PCI_COMMAND, comm.value);
	}

	void enable_bus_mastering(uint64_t address) {
		Command comm = {.value = read_word(address, PCI_COMMAND)};
		comm.attrs.bus_master = true;
		write_word(address, PCI_COMMAND, comm.value);
	}

	void disable_bus_mastering(uint64_t address) {
		Command comm = {.value = read_word(address, PCI_COMMAND)};
		comm.attrs.bus_master = false;
		write_word(address, PCI_COMMAND, comm.value);
	}
}