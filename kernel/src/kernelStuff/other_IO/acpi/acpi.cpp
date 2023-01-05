#include "acpi.h"
#include "../../../OSDATA/osdata.h"

namespace ACPI
{

    void* FindTable(SDTHeader* sdtHeader, char* signature, int div)
    {
        AddToStack();
        SDTHeader* xsdt = sdtHeader;
        int entries = (xsdt->Length - sizeof(ACPI::SDTHeader)) / div;
        //osData.debugTerminalWindow->Log("Entry count: {}", to_string(entries));

        //osData.debugTerminalWindow->renderer->Print("> ");
        for (int t = 0; t < entries; t++)
        {
            uint64_t bleh1 = *(uint64_t*)((uint64_t)xsdt + sizeof(ACPI::SDTHeader) + (t * div));
            if (div == 4)
                bleh1 &= 0x00000000FFFFFFFF;
            ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)bleh1;
            
            for (int i = 0; i < 4; i++)
            {
                if (newSDTHeader->Signature[i] != signature[i])
                {
                    break;
                }
                
                if (i == 3)
                {
                    RemoveFromStack();
                    return newSDTHeader;
                }
            }

            //osData.debugTerminalWindow->renderer->Print(" ");
        }
        //osData.debugTerminalWindow->renderer->Println();
        RemoveFromStack();
        return NULL;
    }

    bool CheckSumHeader(void* start, uint8_t extra, uint32_t len)
    {
        uint8_t val = extra;
        uint8_t* bruh = (uint8_t*)start;
        for (uint32_t i = 0; i < len; i++)
            val += bruh[i];
        return val == 0;
    }

}