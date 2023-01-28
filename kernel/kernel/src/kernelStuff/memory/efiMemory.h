#pragma once
#include <stddef.h>
#include <stdint.h>

struct EFI_MEMORY_DESCRIPTOR
{
    uint32_t type;
    void* physAddr;
    void* virtAddr;
    uint64_t numPages;
    uint64_t attribs;
};

static const char* EFI_MEMORY_TYPE_STRINGS[]
{
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode"
};