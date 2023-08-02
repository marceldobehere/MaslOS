#pragma once
#include "../acpi/acpi.h"


namespace PCI
{
    struct PCIDeviceHeader
    {
        uint16_t Vendor_ID;
        uint16_t Device_ID;
        uint16_t Command;
        uint16_t Status;
        uint8_t Revision_ID;
        uint8_t Prog_IF;
        uint8_t SubClass;
        uint8_t Class;
        uint8_t CacheLineSize;
        uint8_t LatencyTimer;
        uint8_t HeaderType;
        uint8_t BIST;
    };

    struct PCIHeader0 {
        PCIDeviceHeader Header;
        uint32_t BAR0;
        uint32_t BAR1;
        uint32_t BAR2;
        uint32_t BAR3;
        uint32_t BAR4;
        uint32_t BAR5;
        uint32_t CardbusCISPtr;
        uint16_t SubsystemVendorID;
        uint16_t SubsystemID;
        uint32_t ExpansionROMBaseAddr;
        uint8_t CapabilitiesPtr;
        uint8_t Rsv0;
        uint16_t Rsv1;
        uint32_t Rsv2;
        uint8_t InterruptLine;
        uint8_t InterruptPin;
        uint8_t MinGrant;
        uint8_t MaxLatency;
    };

    //PORTS
    #define PCI_ADDRESS_PORT 0xCF8
    #define PCI_DATA_PORT 0xCFC

    // Fields
    #define PCI_VENDOR_ID 0x0 //16
    #define PCI_DEVICE_ID 0x2 //16
    #define PCI_COMMAND 0x4 //16
    #define PCI_STATUS 0x6 //16
    #define PCI_REVISION_ID 0x8 //8
    #define PCI_PROG_IF 0x9 //8
    #define PCI_SUBCLASS 0xa //8
    #define PCI_CLASS 0xb //8
    #define PCI_CACHE_LINE_SIZE 0xc //8
    #define PCI_LATENCY_TIMER 0xd //8
    #define PCI_HEADER_TYPE 0xe //8
    #define PCI_BIST 0xf //8
    #define PCI_BAR0 0x10 //32
    #define PCI_BAR1 0x14 //32
    #define PCI_BAR2 0x18 //32
    #define PCI_BAR3 0x1C //32
    #define PCI_BAR4 0x20 //32
    #define PCI_BAR5 0x24 //32
    #define PCI_PRIMARY_BUS 0x18 //8
    #define PCI_SECONDARY_BUS 0x19 //8
    #define PCI_INTERRUPT_LINE 0x3c //8
    #define PCI_INTERRUPT_PIN 0x3d //8


    //Header types
    #define PCI_MULTIFUNCTION 0x80

    //Vendors
    #define PCI_NONE 0xFFFF

    //Device Classes
    #define PCI_UNCLASSIFIED 0x0
    #define PCI_MASS_STORAGE_CONTROLLER 0x1
    #define PCI_NETWORK_CONTROLLER 0x2
    #define PCI_DISPLAY_CONTROLLER 0x3
    #define PCI_MULTIMEDIA_CONTROLLER 0x4
    #define PCI_MEMORY_CONTROLLER 0x5
    #define PCI_BRIDGE_DEVICE 0x6
    #define PCI_SIMPLE_COMMUNICATION_CONTROLLER 0x7
    #define PCI_BASE_SYSTEM_PERIPHERAL 0x8
    #define PCI_INPUT_DEVICE_CONTROLLER 0x9
    #define PCI_DOCKING_STATION 0xA
    #define PCI_PROCESSOR 0xB
    #define PCI_SERIAL_BUS_CONTROLLER 0xC
    #define PCI_WIRELESS_CONTROLLER 0xD
    #define PCI_INTELLIGENT_CONTROLLER 0xE
    #define PCI_SATELLITE_COMMUNICATION_CONTROLLER 0xF
    #define PCI_ENCRYPTION_CONTROLLER 0x10
    #define PCI_SIGNAL_PROCESSING_CONTROLLER 0x11
    #define PCI_PROCESSING_ACCELERATOR 0x12
    #define PCI_NON_ESSENTIAL_INSTRUMENTATION 0x13
    #define PCI_COPROCESSOR 0x40
    #define PCI_UNASSIGNED_CLASS 0xFF

    //Device Subclasses
    #define PCI_PCI_BRIDGE 0x4
    #define PCI_IDE_CONTROLLER 0x1

    //Device types
    #define PCI_TYPE_BRIDGE 0x0604
    #define PCI_TYPE_IDE_CONTROLLER 0x0101

    void EnumeratePCI(ACPI::MCFGHeader* mcfg);

    void EnumerateBus(uint64_t baseAddress, uint64_t bus);

    void EnumerateDevice(uint64_t busAddress, uint64_t device);

    void EnumerateFunction(uint64_t deviceAddress, uint64_t function);

    extern const char* unknownString;

    const char* GetVendorName(uint16_t vendorID);

    const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID);

    const char* GetClassName(uint8_t classCode);

    const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode);

    const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIFCode);


    uint8_t io_read_byte(uint64_t address, uint8_t field);
	uint16_t io_read_word(uint64_t address, uint8_t field);
	uint32_t io_read_dword(uint64_t address, uint8_t field);

	void io_write_byte(uint64_t address, uint8_t field, uint8_t value);
	void io_write_word(uint64_t address, uint8_t field, uint16_t value);
	void io_write_dword(uint64_t address, uint8_t field, uint32_t value);

	void enable_interrupt(uint64_t address);
	void disable_interrupt(uint64_t address);
	void enable_bus_mastering(uint64_t address);
	void disable_bus_mastering(uint64_t address);
    void enable_io_space(uint64_t address);
	void disable_io_space(uint64_t address);
    void enable_mem_space(uint64_t address);
	void disable_mem_space(uint64_t address);

    // https://github.com/byteduck/duckOS/blob/master/kernel/pci/PCI.h

	union IOAddress {
		struct __attribute((packed)) attrs {
			uint8_t field: 8;
			uint8_t function: 3;
			uint8_t slot: 5;
			uint8_t bus: 8;
			uint8_t : 7;
			bool enable: 1;
		} attrs;
		uint32_t value;
	};

    union Command {
		struct __attribute((packed)) attrs {
			bool io_space : 1;
			bool mem_space : 1;
			bool bus_master : 1;
			bool special_cycles : 1;
			bool mem_write_invalidate_enable : 1;
			bool VGA_palette_snoop : 1;
			bool parity_error_response : 1;
			bool : 1;
			bool serr_enable : 1;
			bool fast_back_to_back_enable : 1;
			bool interrupt_disable : 1;
			uint8_t reserved2 : 5;
		} attrs;
		uint16_t value;
	};

    IOAddress get_address(PCIDeviceHeader* hdr, uint8_t field);
    IOAddress get_address(uint64_t addr, uint8_t field);

    // https://github.com/Glowman554/MicroOS/blob/master/mckrnl/include/driver/pci/pci_bar.h
    enum PCI_BAR_TYPE_ENUM 
    {
        NONE = 0,
        MMIO64,
        MMIO32,
        IO
    };

    struct PCI_BAR_TYPE 
    {
        uint64_t mem_address;
        uint16_t io_address;
        PCI_BAR_TYPE_ENUM type;
        uint16_t size;
    };

    void pci_read_bar(uint32_t* mask, uint64_t addr, uint32_t offset);
    PCI_BAR_TYPE pci_get_bar(uint32_t* bar0, int bar_num, uint64_t addr);
    PCI_BAR_TYPE pci_get_bar(PCIHeader0* addr, int bar_num);

    uint8_t read_byte(uint64_t address, PCI_BAR_TYPE type, uint8_t field);
	uint16_t read_word(uint64_t address, PCI_BAR_TYPE type, uint8_t field);
	uint32_t read_dword(uint64_t address, PCI_BAR_TYPE type, uint8_t field);

	void write_byte(uint64_t address, PCI_BAR_TYPE type, uint16_t field, uint8_t value);
	void write_word(uint64_t address, PCI_BAR_TYPE type, uint16_t field, uint16_t value);
	void write_dword(uint64_t address, PCI_BAR_TYPE type, uint16_t field, uint32_t value);
}