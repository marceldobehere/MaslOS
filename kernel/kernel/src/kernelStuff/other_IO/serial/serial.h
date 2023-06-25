#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Serial
{
    extern bool SerialWorks;
    bool Init();
    bool CanRead();
    char Read();
    bool CanWrite();
    void Write(char chr); 
    void Write(const char* str);
    void Write(const char *chrs, const char *var, bool allowEscape);
}