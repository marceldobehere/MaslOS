#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Serial
{
    extern int SerialPort;
    extern uint64_t pciCard;
    extern bool SerialWorks;
    bool Init();

    bool CanRead();
    char Read();
    bool CanWrite();
    void Write(char chr); 

    bool _CanRead();
    char _Read();
    bool _CanWrite();
    void _Write(char chr); 

    void Write(const char* str);
    void Write(const char *chrs, const char *var);
    void Write(const char *chrs, const char *var, bool allowEscape);
    void Writeln();
    void Writeln(const char* str);
    void Writeln(const char *chrs, const char *var);
    void Writeln(const char *chrs, const char *var, bool allowEscape);

    void Soutb(uint16_t port, uint8_t value);
    uint8_t Sinb(uint16_t port);
}