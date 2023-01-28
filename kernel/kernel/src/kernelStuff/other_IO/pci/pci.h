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
}