#include "serial.h"
#include "../../../OSDATA/osdata.h"

#define PORT 0x3f8          // COM1

namespace Serial
{
    bool SerialWorks = false;
    bool Init()
    {
        AddToStack();
        outb(PORT + 1, 0x00);    // Disable all interrupts
        outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        outb(PORT + 1, 0x00);    //                  (hi byte)
        outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
        outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
        outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
        
        // Check if serial is faulty (i.e: not same byte as sent)
        if(inb(PORT + 0) != 0xAE)
        {
            RemoveFromStack();
            SerialWorks = false;
            return false;
        }
        
        // If serial is not faulty set it in normal operation mode
        // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
        outb(PORT + 4, 0x0F);
        RemoveFromStack();
        SerialWorks = true;
        return true;
    }

    bool CanRead()
    {
        return SerialWorks && (inb(PORT + 5) & 1);
    }

    char Read()
    {
        if (!SerialWorks)
            return 0;
        while (CanRead() == 0);
        return inb(PORT);
    }

    bool CanWrite()
    {
        return SerialWorks && (inb(PORT + 5) & 0x20);
    }

    void Write(char chr)
    {
        if (!SerialWorks)
            return;
        while (CanWrite() == 0);
        outb(PORT, chr);
    }

    void Writeln()
    {
        Write("\r\n");
    }

    void Writeln(const char* str)
    {
        Write(str);
        Writeln();
    }
    void Writeln(const char *chrs, const char *var, bool allowEscape)
    {
        Write(chrs, var, allowEscape);
        Writeln();
    }

    void Write(const char* str)
    {
        if (!SerialWorks || str == 0)
            return;
        while (*str != 0)
        {
            Write(*str);
            str++;
        }
    }

    void Write(const char *chrs, const char *var, bool allowEscape)
    {
        if (!SerialWorks || chrs == 0)
            return;

        unsigned int index = 0;
        while (chrs[index] != 0)
        {
            if (chrs[index] == '\n')
            {
                Serial::Write('\n');
            }
            else if (chrs[index] == '\r')
            {
                Serial::Write('\r');
            }
            else if (chrs[index] == '{' && allowEscape && var != NULL)
            {
                if (chrs[index + 1] == '}')
                {
                    Write(var, NULL, false);
                    index++;
                }
            }
            else if (chrs[index] == '\\' && allowEscape)
            {
                if (chrs[index + 1] == '\\')
                {
                    index++;
                    Write(chrs[index]);
                }
                else if (chrs[index + 1] == '%')
                {
                    index++;
                    Write('%');
                }
                else if (chrs[index + 1] == '{')
                {
                    index++;
                    Write('{');
                }
                else if (chrs[index + 1] == '}')
                {
                    index++;
                    Write('}');
                }
                else if (chrs[index + 1] == 'C')
                {
                    index++;
                    if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                    {
                        Write('?');
                    }
                    else
                    {
                        index++;
                        //color = ConvertStringToHex(&chrs[index]);
                        Write("<Switching Color>");
                        index += 5;
                    }
                }
                else
                {
                    Write(chrs[index]);
                }
            }
            else
            {
                Write(chrs[index]);
            }

            index++;
        }
    }
}