#pragma once
#include <stdint.h>
#include <stddef.h>

namespace FilesystemInterface
{
    enum FilesystemInterfaceType : uint8_t
    {
        None = 0,
        Generic = 1,
        Mrafs = 2
    };

    extern const char* FilesystemInterfaceTypeStr[];
}