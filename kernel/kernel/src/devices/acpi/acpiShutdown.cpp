//
// here is the slighlty complicated ACPI poweroff code
//

#include <stddef.h>
#include "../../osData/osData.h"
#include <string.h>
// #include <io.h>
#include <time.h>

uint32_t *SMI_CMD;
uint8_t ACPI_ENABLE;
uint8_t ACPI_DISABLE;
uint32_t *PM1a_CNT;
uint32_t *PM1b_CNT;
uint16_t SLP_TYPa;
uint16_t SLP_TYPb;
uint16_t SLP_EN;
uint16_t SCI_EN;
uint8_t PM1_CNT_LEN;

struct RSDPtr
{
    uint8_t Signature[8];
    uint8_t CheckSum;
    uint8_t OemID[6];
    uint8_t Revision;
    uint32_t *RsdtAddress;
};

struct FACP
{
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t unneded1[40 - 8];
    uint32_t *DSDT;
    uint8_t unneded2[48 - 44];
    uint32_t *SMI_CMD;
    uint8_t ACPI_ENABLE;
    uint8_t ACPI_DISABLE;
    uint8_t unneded3[64 - 54];
    uint32_t *PM1a_CNT_BLK;
    uint32_t *PM1b_CNT_BLK;
    uint8_t unneded4[89 - 72];
    uint8_t PM1_CNT_LEN;
};












struct GenericAddressStructure
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
};

struct FADT
{
    struct   ACPI::SDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
 
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    // 12 byte structure; see below for details
    GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
 
    GenericAddressStructure X_PM1aEventBlock;
    GenericAddressStructure X_PM1bEventBlock;
    GenericAddressStructure X_PM1aControlBlock;
    GenericAddressStructure X_PM1bControlBlock;
    GenericAddressStructure X_PM2ControlBlock;
    GenericAddressStructure X_PMTimerBlock;
    GenericAddressStructure X_GPE0Block;
    GenericAddressStructure X_GPE1Block;
};








// check if the given address has a valid header
unsigned int *acpiCheckRSDPtr(unsigned int *ptr)
{
    const char *sig = "RSD PTR ";
    struct RSDPtr *rsdp = (struct RSDPtr *)ptr;
    uint8_t *bptr;
    uint8_t check = 0;
    int i;

    if (_memcmp((void *)sig, rsdp, 8) == 0)
    {
        // check checksum rsdpd
        bptr = (uint8_t *)ptr;
        for (i = 0; i < sizeof(struct RSDPtr); i++)
        {
            check += *bptr;
            bptr++;
        }

        // found valid rsdpd
        if (check == 0)
        {
            /*
             if (desc->Revision == 0)
               wrstr("acpi 1");
            else
               wrstr("acpi 2");
            */
            return (unsigned int *)rsdp->RsdtAddress;
        }
    }

    return NULL;
}

// finds the acpi header and returns the address of the rsdt
unsigned int *acpiGetRSDPtr(void)
{
    unsigned int *addr;
    unsigned int *rsdp;

    // search below the 1mb mark for RSDP signature
    for (addr = (unsigned int *)((uint64_t)0x000E0000); (void *)addr < (void *)((uint64_t)0x00100000); addr += 0x10 / sizeof(addr))
    {
        rsdp = acpiCheckRSDPtr(addr);
        if (rsdp != NULL)
            return rsdp;
    }

    // at address 0x40:0x0E is the RM segment of the ebda
    int ebda = *((short *)0x40E);    // get pointer
    ebda = ebda * 0x10 & 0x000FFFFF; // transform segment into linear address

    // search Extended BIOS Data Area for the Root System Description Pointer signature
    for (addr = (unsigned int *)(uint64_t)ebda; (int)(uint64_t)addr < ebda + 1024; addr += 0x10 / sizeof(addr))
    {
        rsdp = acpiCheckRSDPtr(addr);
        if (rsdp != NULL)
            return rsdp;
    }

    return NULL;
}

// checks for a given header and validates checksum
int acpiCheckHeader(unsigned int *ptr, const char *sig)
{
    if (_memcmp(ptr, (void *)sig, 4) == 0)
    {
        char *checkPtr = (char *)ptr;
        int len = *(ptr + 1);
        char check = 0;
        while (0 < len--)
        {
            check += *checkPtr;
            checkPtr++;
        }
        if (check == 0)
            return 0;
    }
    return -1;
}

int acpiEnable(void)
{
    // check if acpi is enabled
    if ((inw((unsigned int)(uint64_t)PM1a_CNT) & SCI_EN) == 0)
    {
        // check if acpi can be enabled
        if (SMI_CMD != 0 && ACPI_ENABLE != 0)
        {
            outb((unsigned int)(uint64_t)SMI_CMD, ACPI_ENABLE); // send acpi enable command
            // give 3 seconds time to enable acpi
            int i;
            for (i = 0; i < 300; i++)
            {
                if ((inw((unsigned int)(uint64_t)PM1a_CNT) & SCI_EN) == 1)
                    break;
                PIT::Sleep(10);
            }
            if (PM1b_CNT != 0)
                for (; i < 300; i++)
                {
                    if ((inw((unsigned int)(uint64_t)PM1b_CNT) & SCI_EN) == 1)
                        break;
                    PIT::Sleep(10);
                }
            if (i < 300)
            {
                GlobalRenderer->Println("enabled acpi.\n", Colors.bgreen);
                return 0;
            }
            else
            {
                GlobalRenderer->Println("couldn't enable acpi.\n", Colors.bred);
                return -1;
            }
        }
        else
        {
            GlobalRenderer->Println("no known way to enable acpi.\n");
            return -1;
        }
    }
    else
    {
        // wrstr("acpi was already enabled.\n");
        return 0;
    }
}

//
// uint8_tcode of the \_S5 object
// -----------------------------------------
//        | (optional) |    |    |    |
// NameOP | \          | _  | S  | 5  | _
// 08     | 5A         | 5F | 53 | 35 | 5F
//
// -----------------------------------------------------------------------------------------------------------
//           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
// PackageOP | PkgLength | NumElements  | uint8_tprefix Num | uint8_tprefix Num | uint8_tprefix Num | uint8_tprefix Num
// 12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05
//
//----this-structure-was-also-seen----------------------
// PackageOP | PkgLength | NumElements |
// 12        | 06        | 04          | 00 00 00 00
//
// (Pkglength bit 6-7 encode additional PkgLength uint8_ts [shouldn't be the case here])
//
int InitAcpiShutdownThing(void *RSDT)
{
    PrintMsgStartLayer("Info");
    PrintMsgCol("RSDT: {}", ConvertHexToString((uint64_t)RSDT), Colors.yellow);
    unsigned int *ptr = (unsigned int *)RSDT; // acpiGetRSDPtr();

    if (RSDT == NULL)
    {
        PrintMsgCol("RSDT IS NULL!", Colors.bred);
        PrintMsgEndLayer("Info");
        return -1;
    }

    FADT* FACP = (FADT*)ACPI::FindTable((ACPI::SDTHeader*)RSDT, "FACP", 4);
    PrintMsgCol("FACP: {}", ConvertHexToString((uint64_t)FACP), Colors.yellow);

    if (FACP == NULL)
    {
        PrintMsgCol("FACP IS NULL!", Colors.bred);
        PrintMsgEndLayer("Info");
        return -1;
    }

    

    //FACP->X_Dsdt

    {
        PrintMsgCol("DSDT: {}", ConvertHexToString((uint64_t)FACP->Dsdt), Colors.yellow);

        // search the \_S5 package in the DSDT
        char *S5Addr = (char *)(uint64_t)FACP->Dsdt + 36; // skip header
        
        int dsdtLength = *((int32_t*)(uint64_t)FACP->Dsdt + 1) - 36;
        

        while (0 < dsdtLength--)
        {
            if (_memcmp(S5Addr, (void *)"_S5_", 4) == 0)
                break;
            S5Addr++;
        }
        // check if \_S5 was found
        if (dsdtLength > 0)
        {
            // check for valid AML structure
            if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12)
            {
                S5Addr += 5;
                S5Addr += ((*S5Addr & 0xC0) >> 6) + 2; // calculate PkgLength size

                if (*S5Addr == 0x0A)
                    S5Addr++; // skip uint8_tprefix
                SLP_TYPa = *(S5Addr) << 10;
                S5Addr++;

                if (*S5Addr == 0x0A)
                    S5Addr++; // skip uint8_tprefix
                SLP_TYPb = *(S5Addr) << 10;

                SMI_CMD = &FACP->SMI_CommandPort;

                ACPI_ENABLE = FACP->AcpiEnable; // either & oder (uint32_t*)
                ACPI_DISABLE = FACP->AcpiDisable;

                PM1a_CNT = (uint32_t*)(uint64_t)FACP->PM1aControlBlock;
                PM1b_CNT = (uint32_t*)(uint64_t)FACP->PM1bControlBlock;

                PM1_CNT_LEN = FACP->PM1ControlLength;

                SLP_EN = 1 << 13;
                SCI_EN = 1;
                PrintMsgEndLayer("Info");
                PrintMsgCol("> Init Complete!", Colors.bgreen);
                return 0;
            }
            else
            {
                PrintMsgEndLayer("Info");
                PrintMsgCol("> S5 parse error.\n", Colors.bred);
            }
        }
        else
        {
            PrintMsgEndLayer("Info");
            PrintMsgCol("> S5 not present.\n", Colors.bred);
        }
    }





    // // check if address is correct  ( if acpi is available on this pc )
    // if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
    // {
    //     // the RSDT contains an unknown number of pointers to acpi tables
    //     int entrys = *(ptr + 1);
    //     entrys = (entrys - 36) / 4;
    //     ptr += 36 / 4; // skip header information

    //     while (0 < entrys--)
    //     {
    //         // check if the desired table is reached
    //         if (acpiCheckHeader((unsigned int *)(uint64_t)*ptr, "FACP") == 0)
    //         {
    //             entrys = -2;
    //             struct FACP *facp = (struct FACP *)(uint64_t)*ptr;
    //             if (acpiCheckHeader((unsigned int *)facp->DSDT, "DSDT") == 0)
    //             {
                    
    //                 // search the \_S5 package in the DSDT
    //                 char *S5Addr = (char *)facp->DSDT + 36; // skip header
    //                 int dsdtLength = *(facp->DSDT + 1) - 36;
    //                 while (0 < dsdtLength--)
    //                 {
    //                     if (_memcmp(S5Addr, (void *)"_S5_", 4) == 0)
    //                         break;
    //                     S5Addr++;
    //                 }
    //                 // check if \_S5 was found
    //                 if (dsdtLength > 0)
    //                 {
    //                     // check for valid AML structure
    //                     if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12)
    //                     {
    //                         S5Addr += 5;
    //                         S5Addr += ((*S5Addr & 0xC0) >> 6) + 2; // calculate PkgLength size

    //                         if (*S5Addr == 0x0A)
    //                             S5Addr++; // skip uint8_tprefix
    //                         SLP_TYPa = *(S5Addr) << 10;
    //                         S5Addr++;

    //                         if (*S5Addr == 0x0A)
    //                             S5Addr++; // skip uint8_tprefix
    //                         SLP_TYPb = *(S5Addr) << 10;

    //                         SMI_CMD = facp->SMI_CMD;

    //                         ACPI_ENABLE = facp->ACPI_ENABLE;
    //                         ACPI_DISABLE = facp->ACPI_DISABLE;

    //                         PM1a_CNT = facp->PM1a_CNT_BLK;
    //                         PM1b_CNT = facp->PM1b_CNT_BLK;

    //                         PM1_CNT_LEN = facp->PM1_CNT_LEN;

    //                         SLP_EN = 1 << 13;
    //                         SCI_EN = 1;

    //                         return 0;
    //                     }
    //                     else
    //                     {
    //                         GlobalRenderer->Println("\\_S5 parse error.\n", Colors.bred);
    //                     }
    //                 }
    //                 else
    //                 {
    //                     GlobalRenderer->Println("\\_S5 not present.\n", Colors.bred);
    //                 }
    //             }
    //             else
    //             {
    //                 GlobalRenderer->Println("DSDT invalid.\n", Colors.bred);
    //             }
    //         }
    //         ptr++;
    //     }
    //     GlobalRenderer->Println("no valid FACP present.\n", Colors.bred);
    // }
    // else
    // {
    //     GlobalRenderer->Println("no acpi.\n", Colors.bred);
    // }

    GlobalRenderer->Println("Init failed!", Colors.bred);
    return -1;
}

void PowerOffAcpi()
{
    // SCI_EN is set to 1 if acpi shutdown is possible
    if (SCI_EN == 0)
        return;

    acpiEnable();

    // send the shutdown command
    outw((unsigned int)(uint64_t)PM1a_CNT, SLP_TYPa | SLP_EN);
    if (PM1b_CNT != 0)
        outw((unsigned int)(uint64_t)PM1b_CNT, SLP_TYPb | SLP_EN);

    GlobalRenderer->Println("ACPI Shutdown failed!\n", Colors.bred);
}