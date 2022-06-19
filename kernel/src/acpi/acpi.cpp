#include "acpi.h"
#include "../OSDATA/osdata.h"

namespace ACPI
{

    void* FindTable(SDTHeader* sdtHeader, char* signature)
    {
        AddToStack("FindTable", "acpi/acpi.cpp");
        SDTHeader* xsdt = sdtHeader;
        int entries = (xsdt->Length - sizeof(ACPI::SDTHeader)) / 8;
        //osData.debugTerminalWindow->Log("Entry count: {}", to_string(entries));

        //osData.debugTerminalWindow->renderer->Print("> ");
        for (int t = 0; t < entries; t++)
        {
            ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)xsdt + sizeof(ACPI::SDTHeader) + (t * 8));
            
            for (int i = 0; i < 4; i++)
            {
                if (newSDTHeader->Signature[i] != signature[i])
                {
                    RemoveFromStack();
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



}