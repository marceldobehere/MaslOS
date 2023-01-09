#pragma once
#include <stdint.h>

namespace ACPI
{
    struct RSDP1
    {
        unsigned char Signature[8];
        uint8_t Checksum;
        uint8_t OEM_ID[6];
        uint8_t Revision;
        uint32_t RSDTAddress;
    } __attribute__((packed));

    struct RSDP2
    {
        RSDP1 firstPart;

        uint32_t Length;
        uint64_t XSDTAddress;
        uint8_t ExtendedChecksum;
        uint8_t Reserved[3];

    } __attribute__((packed));



    struct SDTHeader
    {
        unsigned char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        uint8_t OEM_ID[6];
        uint8_t OEM_TABLE_ID[8];
        uint32_t OEM_Revision;
        uint32_t Creator_ID; 
        uint32_t CreatorRevision;
    } __attribute__((packed));

    struct MCFGHeader
    {
        SDTHeader Header;
        uint64_t Reserved;
    } __attribute__((packed));

    struct DeviceConfig
    {
        uint64_t BaseAddress;
        uint16_t PCISegGroup;
        uint8_t StartBus;
        uint8_t EndBus;
        uint32_t Reserved;
    } __attribute__((packed));


    void* FindTable(SDTHeader* sdtHeader, char* signature, int div);
    bool CheckSumHeader(void* start, uint8_t extra, uint32_t len);

}  