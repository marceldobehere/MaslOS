#include "paging.h"

void* GetPhysAddr(void *virtualaddr)
{
    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
    unsigned long *pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
 
    unsigned long *pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
 
    return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}

void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
 
void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi)
{
   asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

// PAT IS AT 0x277
// PA 5 (which is 40-42) (high bit 8 to 10) which we need to set to 0b001 so WC

void doMagicWithPAT()
{
    uint32_t lo = 0;
    uint32_t hi = 0;
    cpuGetMSR(0x277, &lo, &hi);

    hi &= 0b0001111111;
    hi |= 0b0010000000;
    cpuSetMSR(0x277, lo, hi);   
}