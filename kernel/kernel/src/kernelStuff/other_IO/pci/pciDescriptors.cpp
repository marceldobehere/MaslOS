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












    const int _IdeControllerProgIFCount = 8;
    const ProgIFEntry _IdeControllerProgIFs[] {
        {0x0, "ISA Compatibility mode-only controller"},
        {0x5, "PCI native mode-only controller"},
        {0xA, "ISA Compatibility mode controller, supports both channels switched to PCI native mode"},
        {0xF, "PCI native mode controller, supports both channels switched to ISA compatibility mode"},
        {0x80, "ISA Compatibility mode-only controller, supports bus mastering"},
        {0x85, "PCI native mode-only controller, supports bus mastering"},
        {0x8A, "ISA Compatibility mode controller, supports both channels switched to PCI native mode, supports bus mastering"},
        {0x8F, "PCI native mode controller, supports both channels switched to ISA compatibility mode, supports bus mastering"},
    };

    const int _AtaControllerProgIFCount = 2;
    const ProgIFEntry _AtaControllerProgIFs[] {
        {0x20, "Single DMA"},
        {0x30, "Chained DMA"},
    };

    const int _SerialAtaControllerProgIFCount = 3;
    const ProgIFEntry _SerialAtaControllerProgIFs[] {
        {0x0, "Vendor Specific Interface"},
        {0x1, "AHCI 1.0"},
        {0x2, "Serial Storage Bus"},
    };

    const int _SerialAttachedScsiControllerProgIFCount = 2;
    const ProgIFEntry _SerialAttachedScsiControllerProgIFs[] {
        {0x0, "SAS"},
        {0x1, "Serial Storage Bus"},
    };

    const int _NonVolatileMemoryControllerProgIFCount = 2;
    const ProgIFEntry _NonVolatileMemoryControllerProgIFs[] {
        {0x1, "NVMHCI"},
        {0x2, "NVM Express"},
    };

    const int _VgaCompatibleControllerProgIFCount = 2;
    const ProgIFEntry _VgaCompatibleControllerProgIFs[] {
        {0x0, "VGA Controller"},
        {0x1, "8514-Compatible Controller"},
    };

    const int _PciToPciBridgeProgIFCount = 2;
    const ProgIFEntry _PciToPciBridgeProgIFs[] {
        {0x0, "Normal Decode"},
        {0x1, "Subtractive Decode"},
    };

    const int _RacewayBridgeProgIFCount = 2;
    const ProgIFEntry _RacewayBridgeProgIFs[] {
        {0x0, "Transparent Mode"},
        {0x1, "Endpoint Mode"},
    };

    const int _PciToPciBridge2ProgIFCount = 2;
    const ProgIFEntry _PciToPciBridge2ProgIFs[] {
        {0x40, "Semi-Transparent, Primary bus towards host CPU"},
        {0x80, "Semi-Transparent, Secondary bus towards host CPU"},
    };

    const int _SerialControllerProgIFCount = 7;
    const ProgIFEntry _SerialControllerProgIFs[] {
        {0x0, "8250-Compatible (Generic XT)"},
        {0x1, "16450-Compatible"},
        {0x2, "16550-Compatible"},
        {0x3, "16650-Compatible"},
        {0x4, "16750-Compatible"},
        {0x5, "16850-Compatible"},
        {0x6, "16950-Compatible"},
    };

    const int _ParallelControllerProgIFCount = 5;
    const ProgIFEntry _ParallelControllerProgIFs[] {
        {0x0, "Standard Parallel Port"},
        {0x1, "Bi-Directional Parallel Port"},
        {0x2, "ECP 1.X Compliant Parallel Port"},
        {0x3, "IEEE 1284 Controller"},
        {0xFE, "IEEE 1284 Target Device"},
    };

    const int _ModemProgIFCount = 5;
    const ProgIFEntry _ModemProgIFs[] {
        {0x0, "Generic Modem"},
        {0x1, "Hayes 16450-Compatible Interface"},
        {0x2, "Hayes 16550-Compatible Interface"},
        {0x3, "Hayes 16650-Compatible Interface"},
        {0x4, "Hayes 16750-Compatible Interface"},
    };

    const int _PicProgIFCount = 5;
    const ProgIFEntry _PicProgIFs[] {
        {0x0, "Generic 8259-Compatible"},
        {0x1, "ISA-Compatible"},
        {0x2, "EISA-Compatible"},
        {0x10, "I/O APIC Interrupt Controller"},
        {0x20, "I/O(x) APIC Interrupt Controller"},
    };

    const int _DmaControllerProgIFCount = 3;
    const ProgIFEntry _DmaControllerProgIFs[] {
        {0x00, "Generic 8237-Compatible"},
        {0x01, "ISA-Compatible"},
        {0x02, "EISA-Compatible"},
    };

    const int _TimerProgIFCount = 4;
    const ProgIFEntry _TimerProgIFs[] {
        {0x00, "Generic 8254-Compatible"},
        {0x01, "ISA-Compatible"},
        {0x02, "EISA-Compatible"},
        {0x03, "HPET"},
    };

    const int _RtcControllerProgIFCount = 2;
    const ProgIFEntry _RtcControllerProgIFs[] {
        {0x0, "Generic RTC"},
        {0x1, "ISA-Compatible"},
    };

    const int _GameportControllerProgIFCount = 2;
    const ProgIFEntry _GameportControllerProgIFs[] {
        {0x0, "Generic"},
        {0x10, "Extended"},
    };

    const int _FirewireIeee1394ControllerProgIFCount = 2;
    const ProgIFEntry _FirewireIeee1394ControllerProgIFs[] {
        {0x0, "Generic"},
        {0x10, "OHCI"},
    };

    const int _UsbControllerProgIFCount = 6;
    const ProgIFEntry _UsbControllerProgIFs[] {
        {0x0, "UHCI Controller"},
        {0x10, "OHCI Controller"},
        {0x20, "EHCI (USB2) Controller"},
        {0x30, "XHCI (USB3) Controller"},
        {0x80, "Unspecified"},
        {0xFE, "USB Device (Not a host controller)"},
    };

    const int _IpmiInterfaceProgIFCount = 3;
    const ProgIFEntry _IpmiInterfaceProgIFs[] {
        {0x0, "SMIC"},
        {0x1, "Keyboard Controller Style"},
        {0x2, "Block Transfer"},
    };

    const int _UnclassifiedSubclassCount = 2;
    const SubclassEntry _UnclassifiedSubclasses[] {
        {0x0, "Non-VGA-Compatible Unclassified Device", 0, 0},
        {0x1, "VGA-Compatible Unclassified Device", 0, 0},
    };

    const int _MassStorageControllerSubclassCount = 10;
    const SubclassEntry _MassStorageControllerSubclasses[] {
        {0x0, "SCSI Bus Controller", 0, 0},
        {0x1, "IDE Controller", _IdeControllerProgIFCount, _IdeControllerProgIFs},
        {0x2, "Floppy Disk Controller", 0, 0},
        {0x3, "IPI Bus Controller", 0, 0},
        {0x4, "RAID Controller", 0, 0},
        {0x5, "ATA Controller", _AtaControllerProgIFCount, _AtaControllerProgIFs},
        {0x6, "Serial ATA Controller", _SerialAtaControllerProgIFCount, _SerialAtaControllerProgIFs},
        {0x7, "Serial Attached SCSI Controller", _SerialAttachedScsiControllerProgIFCount, _SerialAttachedScsiControllerProgIFs},
        {0x8, "Non-Volatile Memory Controller", _NonVolatileMemoryControllerProgIFCount, _NonVolatileMemoryControllerProgIFs},
        {0x80, "Other", 0, 0},
    };

    const int _NetworkControllerSubclassCount = 10;
    const SubclassEntry _NetworkControllerSubclasses[] {
        {0x0, "Ethernet Controller", 0, 0},
        {0x1, "Token Ring Controller", 0, 0},
        {0x2, "FDDI Controller", 0, 0},
        {0x3, "ATM Controller", 0, 0},
        {0x4, "ISDN Controller", 0, 0},
        {0x5, "WorldFip Controller", 0, 0},
        {0x6, "PICMG 2.14 Multi Computing Controller", 0, 0},
        {0x7, "Infiniband Controller", 0, 0},
        {0x8, "Fabric Controller", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _DisplayControllerSubclassCount = 4;
    const SubclassEntry _DisplayControllerSubclasses[] {
        {0x0, "VGA Compatible Controller", _VgaCompatibleControllerProgIFCount, _VgaCompatibleControllerProgIFs},
        {0x1, "XGA Controller", 0, 0},
        {0x2, "3D Controller (Not VGA-Compatible)", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _MultimediaControllerSubclassCount = 5;
    const SubclassEntry _MultimediaControllerSubclasses[] {
        {0x0, "Multimedia Video Controller", 0, 0}, 
        {0x1, "Multimedia Audio Controller", 0, 0},
        {0x2, "Computer Telephony Device", 0, 0},
        {0x3, "Audio Device", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _MemoryControllerSubclassCount = 3;
    const SubclassEntry _MemoryControllerSubclasses[] {
        {0x0, "RAM Controller", 0, 0},
        {0x1, "Flash Controller", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _BridgeSubclassCount = 12;
    const SubclassEntry _BridgeSubclasses[] {
        {0x0, "Host Bridge", 0, 0},
        {0x1, "ISA Bridge", 0, 0},
        {0x2, "EISA Bridge", 0, 0},
        {0x3, "MCA Bridge", 0, 0},
        {0x4, "PCI-to-PCI Bridge", _PciToPciBridgeProgIFCount, _PciToPciBridgeProgIFs},
        {0x5, "PCMCIA Bridge", 0, 0},
        {0x6, "NuBus Bridge", 0, 0},
        {0x7, "CardBus Bridge", 0, 0},
        {0x8, "RACEway Bridge", _RacewayBridgeProgIFCount, _RacewayBridgeProgIFs},
        {0x9, "PCI-to-PCI Bridge", _PciToPciBridge2ProgIFCount, _PciToPciBridge2ProgIFs},
        {0x0A, "InfiniBand-to-PCI Host Bridge", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _SimpleCommunicationControllerSubclassCount = 7;
    const SubclassEntry _SimpleCommunicationControllerSubclasses[] {
        {0x0, "Serial Controller", _SerialControllerProgIFCount, _SerialControllerProgIFs},
        {0x1, "Parallel Controller", _ParallelControllerProgIFCount, _ParallelControllerProgIFs},
        {0x2, "Multiport Serial Controller", 0, 0},
        {0x3, "Modem", _ModemProgIFCount, _ModemProgIFs},
        {0x4, "IEEE 488.1/2 (GPIB) Controller", 0, 0},
        {0x5, "Smart Card Controller", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _BaseSystemPeripheralSubclassCount = 8;
    const SubclassEntry _BaseSystemPeripheralSubclasses[] {
        {0x0, "PIC", _PicProgIFCount, _PicProgIFs},
        {0x01, "DMA Controller", _DmaControllerProgIFCount, _DmaControllerProgIFs},
        {0x02, "Timer", _TimerProgIFCount, _TimerProgIFs},
        {0x3, "RTC Controller", _RtcControllerProgIFCount, _RtcControllerProgIFs},
        {0x4, "PCI Hot-Plug Controller", 0, 0},
        {0x5, "SD Host controller", 0, 0},
        {0x6, "IOMMU", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _InputDeviceControllerSubclassCount = 6;
    const SubclassEntry _InputDeviceControllerSubclasses[] {
        {0x0, "Keyboard Controller", 0, 0},
        {0x1, "Digitizer Pen", 0, 0},
        {0x2, "Mouse Controller", 0, 0},
        {0x3, "Scanner Controller", 0, 0},
        {0x4, "Gameport Controller", _GameportControllerProgIFCount, _GameportControllerProgIFs},
        {0x80, "Other", 0, 0},
    };

    const int _DockingStationSubclassCount = 2;
    const SubclassEntry _DockingStationSubclasses[] {
        {0x0, "Generic", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _ProcessorSubclassCount = 9;
    const SubclassEntry _ProcessorSubclasses[] {
        {0x0, "386", 0, 0},
        {0x1, "486", 0, 0},
        {0x2, "Pentium", 0, 0},
        {0x3, "Pentium Pro", 0, 0},
        {0x10, "Alpha", 0, 0},
        {0x20, "PowerPC", 0, 0},
        {0x30, "MIPS", 0, 0},
        {0x40, "Co-Processor", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _SerialBusControllerSubclassCount = 11;
    const SubclassEntry _SerialBusControllerSubclasses[] {
        {0x0, "FireWire (IEEE 1394) Controller", _FirewireIeee1394ControllerProgIFCount, _FirewireIeee1394ControllerProgIFs},
        {0x1, "ACCESS Bus Controller", 0, 0},
        {0x2, "SSA", 0, 0},
        {0x3, "USB Controller", _UsbControllerProgIFCount, _UsbControllerProgIFs},
        {0x4, "Fibre Channel", 0, 0},
        {0x5, "SMBus Controller", 0, 0},
        {0x6, "InfiniBand Controller", 0, 0},
        {0x7, "IPMI Interface", _IpmiInterfaceProgIFCount, _IpmiInterfaceProgIFs},
        {0x8, "SERCOS Interface (IEC 61491)", 0, 0},
        {0x9, "CANbus Controller", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _WirelessControllerSubclassCount = 8;
    const SubclassEntry _WirelessControllerSubclasses[] {
        {0x0, "iRDA Compatible Controller", 0, 0},
        {0x1, "Consumer IR Controller", 0, 0},
        {0x10, "RF Controller", 0, 0},
        {0x11, "Bluetooth Controller", 0, 0},
        {0x12, "Broadband Controller", 0, 0},
        {0x20, "Ethernet Controller (802.1a)", 0, 0},
        {0x21, "Ethernet Controller (802.1b)", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _IntelligentControllerSubclassCount = 1;
    const SubclassEntry _IntelligentControllerSubclasses[] {
        {0x0, "I20", 0, 0},
    };

    const int _SatelliteCommunicationControllerSubclassCount = 4;
    const SubclassEntry _SatelliteCommunicationControllerSubclasses[] {
        {0x1, "Satellite TV Controller", 0, 0},
        {0x2, "Satellite Audio Controller", 0, 0},
        {0x3, "Satellite Voice Controller", 0, 0},
        {0x4, "Satellite Data Controller", 0, 0},
    };

    const int _EncryptionControllerSubclassCount = 3;
    const SubclassEntry _EncryptionControllerSubclasses[] {
        {0x0, "Network and Computing Encrpytion/Decryption", 0, 0},
        {0x10, "Entertainment Encryption/Decryption", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int _SignalProcessingControllerSubclassCount = 5;
    const SubclassEntry _SignalProcessingControllerSubclasses[] {
        {0x0, "DPIO Modules", 0, 0},
        {0x1, "Performance Counters", 0, 0},
        {0x10, "Communication Synchronizer", 0, 0},
        {0x20, "Signal Processing Management", 0, 0},
        {0x80, "Other", 0, 0},
    };

    const int implementedClassCount = 24;
    const ClassEntry implementedClasses[] {
        {0x0, "Unclassified", _UnclassifiedSubclassCount, _UnclassifiedSubclasses},
        {0x1, "Mass Storage Controller", _MassStorageControllerSubclassCount, _MassStorageControllerSubclasses},
        {0x2, "Network Controller", _NetworkControllerSubclassCount, _NetworkControllerSubclasses},
        {0x3, "Display Controller", _DisplayControllerSubclassCount, _DisplayControllerSubclasses},
        {0x4, "Multimedia Controller", _MultimediaControllerSubclassCount, _MultimediaControllerSubclasses},
        {0x5, "Memory Controller", _MemoryControllerSubclassCount, _MemoryControllerSubclasses},
        {0x6, "Bridge", _BridgeSubclassCount, _BridgeSubclasses},
        {0x7, "Simple Communication Controller", _SimpleCommunicationControllerSubclassCount, _SimpleCommunicationControllerSubclasses},
        {0x8, "Base System Peripheral", _BaseSystemPeripheralSubclassCount, _BaseSystemPeripheralSubclasses},
        {0x9, "Input Device Controller", _InputDeviceControllerSubclassCount, _InputDeviceControllerSubclasses},
        {0xA, "Docking Station", _DockingStationSubclassCount, _DockingStationSubclasses},
        {0xB, "Processor", _ProcessorSubclassCount, _ProcessorSubclasses},
        {0xC, "Serial Bus Controller", _SerialBusControllerSubclassCount, _SerialBusControllerSubclasses},
        {0xD, "Wireless Controller", _WirelessControllerSubclassCount, _WirelessControllerSubclasses},
        {0xE, "Intelligent Controller", _IntelligentControllerSubclassCount, _IntelligentControllerSubclasses},
        {0xF, "Satellite Communication Controller", _SatelliteCommunicationControllerSubclassCount, _SatelliteCommunicationControllerSubclasses},
        {0x10, "Encryption Controller", _EncryptionControllerSubclassCount, _EncryptionControllerSubclasses},
        {0x11, "Signal Processing Controller", _SignalProcessingControllerSubclassCount, _SignalProcessingControllerSubclasses},
        {0x12, "Processing Accelerator", 0, 0},
        {0x13, "Non-Essential Instrumentation", 0, 0},
        {0x14, "0x3F (Reserved)", 0, 0},
        {0x40, "Co-Processor", 0, 0},
        {0x41, "0xFE (Reserved)", 0, 0},
        {0xFF, "Unassigned Class (Vendor specific)", 0, 0},
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