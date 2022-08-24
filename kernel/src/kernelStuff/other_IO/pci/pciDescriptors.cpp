#include <stdint.h>
//#include "cstr.h"

namespace PCI {

    struct ProgIFEntry
    {
        uint32_t id;
        const char* name; 
    };

    struct SubclassEntry
    {
        uint32_t id;
        const char* name;  
        int32_t subProgIFCount;
        const ProgIFEntry* subProgIFs;
    };

    struct ClassEntry
    {
        uint32_t id;
        const char* name;  
        int32_t subclassCount;
        const SubclassEntry* subclasses;
    };

    struct DeviceEntry
    {
        uint32_t id;
        const char* name; 
    };

    struct VendorEntry
    {
        uint32_t id;
        const char* name;  
        int32_t deviceCount;
        const DeviceEntry* devices;
    };






    //  Add Stuff using
    // www.pcilookup.com
    // or using
    // wiki.osdev.org/PCI





    const int intelDeviceCount = 4;
    const DeviceEntry intelDevices[] = {
        {0x29C0, "Express DRAM Controller"},
        {0x2918, "LPC Interface Controller"},
        {0x2922, "6 Port SATA Controller"},
        {0x2930, "SMBus Controller"}
    };
    const int amdDeviceCount = 0;
    const DeviceEntry amdDevices[] = {
        {0x0000, "Test"}
    };
    const int nvidiaDeviceCount = 0;
    const DeviceEntry nvidiaDevices[] = {
        {0x0000, "Test"}
    };
    const int qemuDeviceCount = 0;
    const DeviceEntry qemuDevices[] = {
        {0x0000, "Test"}
    };

    const int implementedVendorCount = 4;
    const VendorEntry implementedVendors[] {
        {0x8086, "Intel Corp", intelDeviceCount, intelDevices},
        {0x1022, "AMD Corp", amdDeviceCount, amdDevices},
        {0x10DE, "NVIDIA Corp", nvidiaDeviceCount, nvidiaDevices},
        {0x1234, "QEMU Corp", qemuDeviceCount, qemuDevices}
    };

    const int serialATAProgIFCount = 3;
    const ProgIFEntry serialATAProgIFs[] = {
        {0x00, "Vendor Specific Interface"},
        {0x01, "AHCI 1.0"},
        {0x02, "Serial Storage Bus"}
    };

    const int massStorageSubclassCount = 10;
    const SubclassEntry massStorageSubclasses[] = {
        {0x00, "SCSI Bus Controller", 0, 0},
        {0x01, "IDE Controller", 0, 0},
        {0x02, "Floppy Disk Controller", 0, 0},
        {0x03, "IPI Bus Controller", 0, 0},
        {0x04, "RAID Controller", 0, 0},
        {0x05, "ATA Controller", 0, 0},
        {0x06, "Serial ATA", serialATAProgIFCount, serialATAProgIFs},
        {0x07, "Serial Attached SCSI", 0, 0},
        {0x08, "Non-Volatile Memory Controller", 0, 0},
        {0x80, "Other", 0, 0}
    };

    const int unclassifiedSubclassCount = 2;
    const SubclassEntry unclassifiedSubclasses[] = {
        {0x00, "Non-VGA-Compatible Unclassified Device", 0, 0},
        {0x01, "VGA-Compatible Unclassified Device", 0, 0},
    };

    const int bridgeSubclassCount = 12;
    const SubclassEntry bridgeSubclasses[] = {
        {0x00, "Host Bridge", 0, 0},
        {0x01, "ISA Bridge", 0, 0},
        {0x02, "EISA Bridge", 0, 0},
        {0x03, "MCA Bridge", 0, 0},
        {0x04, "PCI-to-PCI Bridge", 0, 0},
        {0x05, "PCMCIA Bridge", 0, 0},
        {0x06, "NuBus Bridge", 0, 0},
        {0x07, "CardBus Bridge", 0, 0},
        {0x08, "RACEway Bridge", 0, 0},
        {0x09, "PCI-to-PCI Bridge", 0, 0},
        {0x0A, "InfiniBand-to-PCI Host Bridge", 0, 0},
        {0x80, "Other", 0, 0},
    };


    const int vgaProgIFCount = 2;
    const ProgIFEntry vgaProgIFs[] = {
        {0x00, "VGA Controller"},
        {0x01, "8514-Compatible Controller"},
    };

    const int displaySubclassCount = 4;
    const SubclassEntry displaySubclasses[] = {
        {0x00, "VGA-Compatible Controller", vgaProgIFCount, vgaProgIFs},
        {0x01, "XGA Controller", 0, 0},
        {0x02, "3D Controller (Not VGA-Compatible)", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int serialBusSubclassCount = 11;
    const SubclassEntry serialBusSubclasses[] = {
        {0x00, "FireWire (IEEE 1394) Controller", 0, 0},
        {0x01, "ACCESS Bus Controller", 0, 0},
        {0x02, "SSA", 0, 0},
        {0x03, "USB Controller", 0, 0},
        {0x04, "Fibre Channel", 0, 0},
        {0x05, "SMBus Controller", 0, 0},
        {0x06, "InfiniBand Controller", 0, 0},
        {0x07, "IPMI Controller", 0, 0},
        {0x08, "SERCOS Interface (IEC 61491)", 0, 0},
        {0x09, "CANbus Controller", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int implementedClassCount = 19;
    const ClassEntry implementedClasses[] {
        {0x00, "Unclassified", unclassifiedSubclassCount, unclassifiedSubclasses},
        {0x01, "Mass Storage Controller", massStorageSubclassCount, massStorageSubclasses},
        {0x02, "Network Controller", 0, 0},
        {0x03, "Display Controller", displaySubclassCount, displaySubclasses},
        {0x04, "Multimedia Controller", 0, 0},
        {0x05, "Memory Controller", 0, 0},
        {0x06, "Bridge Device", bridgeSubclassCount, bridgeSubclasses},
        {0x07, "Simple Communication Controller", 0, 0},
        {0x08, "Base System Peripheral", 0, 0},
        {0x09, "Input Device Controller", 0, 0},
        {0x0A, "Docking Station", 0, 0},
        {0x0B, "Processor", 0, 0},
        {0x0C, "Serial Bus Controller", serialBusSubclassCount, serialBusSubclasses},
        {0x0D, "Wireless Controller", 0, 0},
        {0x0E, "Satellite Communication Controller", 0, 0},
        {0x0F, "Encryption Controller", 0, 0},
        {0x10, "Signal Processing Controller", 0, 0},
        {0x11, "Proccesing Accelerator", 0, 0},
        {0x12, "Non Essential Instrumentation", 0, 0},
    };






























    const char* unknownString = "<UNKNOWN>";

    const VendorEntry* GetVendorEntry(uint16_t vendorID)
    {
        for (int i = 0; i < implementedVendorCount; i++)
            if (implementedVendors[i].id == vendorID)
                return &implementedVendors[i];

        return (VendorEntry*)0;
    }

    const DeviceEntry* GetDeviceEntry(const VendorEntry* vendor, uint16_t deviceID)
    {
        for (int i = 0; i < vendor->deviceCount; i++)
            if (vendor->devices[i].id == deviceID)
                return &vendor->devices[i];

        return (DeviceEntry*)0;
    }

    const ClassEntry* GetClassEntry(uint8_t classID)
    {
        for (int i = 0; i < implementedClassCount; i++)
            if (implementedClasses[i].id == classID)
                return &implementedClasses[i];

        return (ClassEntry*)0;
    }

    const SubclassEntry* GetSubclassEntry(const ClassEntry* classEntry, uint16_t subclassID)
    {
        for (int i = 0; i < classEntry->subclassCount; i++)
            if (classEntry->subclasses[i].id == subclassID)
                return &classEntry->subclasses[i];

        return (SubclassEntry*)0;
    }

    const ProgIFEntry* GetProgIFEntry(const SubclassEntry* subclassEntry, uint16_t progIFID)
    {
        for (int i = 0; i < subclassEntry->subProgIFCount; i++)
            if (subclassEntry->subProgIFs[i].id == progIFID)
                return &subclassEntry->subProgIFs[i];

        return (ProgIFEntry*)0;
    }


    const char* GetVendorName(uint16_t vendorID)
    {
        const VendorEntry* vendor = GetVendorEntry(vendorID);
        if (vendor == 0)
            return unknownString;
        return vendor->name;
    }

    const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID)
    {
        const VendorEntry* vendor = GetVendorEntry(vendorID);
        if (vendor == 0)
            return unknownString;

        const DeviceEntry* device = GetDeviceEntry(vendor, deviceID);
        if (device == 0)
            return unknownString;

        return device->name;
    }

    const char* GetClassName(uint8_t classCode)
    {
        const ClassEntry* classEntry = GetClassEntry(classCode);
        if (classEntry == 0)
            return unknownString;
        
        return classEntry->name;
    }

    const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode)
    {
        const ClassEntry* classEntry = GetClassEntry(classCode);
        if (classEntry == 0)
            return unknownString;
        
        const SubclassEntry* subclassEntry = GetSubclassEntry(classEntry, subclassCode);
        if (subclassEntry == 0)
            return unknownString;

        return subclassEntry->name;
    }

    const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIFCode)
    {
        const ClassEntry* classEntry = GetClassEntry(classCode);
        if (classEntry == 0)
            return unknownString;
        
        const SubclassEntry* subclassEntry = GetSubclassEntry(classEntry, subclassCode);
        if (subclassEntry == 0)
            return unknownString;

        const ProgIFEntry* progIFEntry = GetProgIFEntry(subclassEntry, progIFCode);
        if (progIFEntry == 0)
            return unknownString;

        return progIFEntry->name;
    }
}