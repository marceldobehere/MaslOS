#include "serial.h"
#include "../../osData/osData.h"


namespace Serial
{
    int SerialPort = 0x3F8;          // COM1
    uint64_t pciCard = 0;
    bool SerialWorks = false;
    uint16_t pciIoBase = 0;
    SerialManager::GenericPacket* currentSerialReadPacket = NULL;
    int currentSerialReadPacketIndex = 0;

    bool Init()
    {
        AddToStack();

        if (pciCard != 0)
        {
            osData.debugTerminalWindow->Log("Serial PCI CARD AT: 0x{}", ConvertHexToString(pciCard), Colors.yellow);
            uint64_t bar0 = ((PCI::PCIHeader0*)pciCard)->BAR0;
            uint64_t bar1 = ((PCI::PCIHeader0*)pciCard)->BAR1;
            uint64_t bar2 = ((PCI::PCIHeader0*)pciCard)->BAR2;
            osData.debugTerminalWindow->Log("Serial PCI CARD BAR0: {}", ConvertHexToString(bar0), Colors.yellow);
            //osData.debugTerminalWindow->Log("Serial PCI CARD BAR2: {}", ConvertHexToString(bar2), Colors.yellow);
            //uint32_t ret = PCI::read_word(pciCard, PCI_BAR0);
            //osData.debugTerminalWindow->Log("Serial PCI CARD BAR0 (2): {}", ConvertHexToString(ret), Colors.yellow);
            //ret = PCI::read_word(pciCard, PCI_BAR2);
            //osData.debugTerminalWindow->Log("Serial PCI CARD BAR2 (2): {}", ConvertHexToString(ret), Colors.yellow);
            pciIoBase = bar0;// & (~0x3);
            //pciIoBase = bar2;// & (~0x3);
            pciIoBase += 0xC0;


            // try with membase?
            //pciIoBase = bar0 + 0x3A7;

            //pciIoBase += 0x08;
            osData.debugTerminalWindow->Log("Serial PCI CARD IO BASE: {}", to_string(pciIoBase), Colors.bgreen);
            PCI::enable_interrupt(pciCard);
            PCI::enable_bus_mastering(pciCard);

            // Soutb(1, 0x80);
            // io_wait(100);
        }
        else
        {

        }
        
        // if (pciCard == 0)
        // {
        //     // Disable all interrupts
        //     Soutb(1, 0x00);
        //     // Enable DLAB (set baud rate divisor)
        //     Soutb(3, 0x80);
        //     // Set divisor to 3 (lo byte) 38400 baud
        //     Soutb(0, 0x03);
        //     //                  (hi byte)
        //     Soutb(1, 0x00);
        //     // 8 bits, no parity, one stop bit
        //     Soutb(3, 0x03);
        //     // Enable FIFO, clear them, with 14-byte threshold
        //     Soutb(2, 0xC7);
        //     // IRQs enabled, RTS/DSR set
        //     Soutb(4, 0x0B);
            
        // }
        // else
        // {
        //     // Soutb(3, 0x03);   // Set word size
        //     // Soutb(1, 0x80);    // Reset
        //     // Soutb(3, 0x80);    // Enable DLAB (set baud rate divisor)
        //     // Soutb(0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        //     // Soutb(1, 0x00);    //                  (hi byte)
        //     // Soutb(4, 0x80);    // half duplex ig
        //     // Soutb(3, 0x03);    // 8 bits, no parity, one stop bit
        //     // Soutb(1, 0x00);    // Disable all interrupts
        //     // //Soutb(2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        //     // Soutb(2, 0x00);    // No FIFO
        //     // //Soutb(4, 0x0B);    // IRQs enabled, RTS/DSR set
        //     // Soutb(4, 0x08);    // OUT yes

        //     // Disable all interrupts
        //     Soutb(1, 0x00);
        //     // Enable DLAB (set baud rate divisor)
        //     Soutb(3, 0x80);
        //     // Set divisor to 3 (lo byte) 38400 baud
        //     Soutb(0, 0x03);
        //     //                  (hi byte)
        //     Soutb(1, 0x00);
        //     // 8 bits, no parity, one stop bit
        //     Soutb(3, 0x03);
        //     // Enable FIFO, clear them, with 14-byte threshold
        //     Soutb(2, 0xC7);
        //     // IRQs enabled, RTS/DSR set
        //     Soutb(4, 0x0B);

        // }

        {
            // Disable all interrupts
            Soutb(1, 0x00);
            io_wait(100);
            // Enable DLAB (set baud rate divisor)
            Soutb(3, 0x80);
            io_wait(100);
            // Set divisor to 3 (lo byte) 38400 baud
            Soutb(0, 0x01); // NVM WE DO 1 -> 115200 baud
            //                  (hi byte)
            Soutb(1, 0x00);
            io_wait(100);
            // 8 bits, no parity, one stop bit
            Soutb(3, 0x03);
            // Enable FIFO, clear them, with 14-byte threshold
            // actually looking at the data sheet its 224 bytes apparently
            Soutb(2, 0xC7);
            // IRQs enabled, RTS/DSR set
            Soutb(4, 0x0B);
            io_wait(100);
        }
        
        for (int i = 0; i < 5000 && Sinb(5) & 1 == 1; i++)
            Sinb(0);
        
        Soutb(4, 0x1E);    // Set in loopback mode, test the serial chip
        
        for (int i = 0; i < 5000 && Sinb(5) & 1 == 1; i++)
            Sinb(0);
        
        Soutb(0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
        io_wait(500);
        // Check if serial is faulty (i.e: not same byte as sent)
        if(Sinb(0) != 0xAE)
        {
            if (osData.debugTerminalWindow != NULL)
            {
                if (pciCard != 0)
                    osData.debugTerminalWindow->Log("Serial PCI NO WORK :(!");
                else
                    osData.debugTerminalWindow->Log("Serial Port NO WORK :(!");
            }
            RemoveFromStack();
            SerialWorks = false;
            return false;
        }
        else
        {
            if (pciCard != 0)
                osData.debugTerminalWindow->Log("Serial PCI WORKS! :)");
        }
        
        // If serial is not faulty set it in normal operation mode
        // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
        // if (pciCard == 0)
        //     Soutb(4, 0x0F);
        // else
        //     Soutb(4, 0x03);    // OUT yes

        io_wait(100);
        Soutb(4, 0x0F);
        RemoveFromStack();
        SerialWorks = true;
        currentSerialReadPacket = NULL;
        currentSerialReadPacketIndex = 0;
        return true;
    }

    bool CanRead()
    {
        if (osData.serialManager == NULL)
            return _CanRead();
        else
        {
            if (currentSerialReadPacket != NULL)
                return true;
            if (osData.serialManager->clientConnected)    
                return osData.serialManager->HasPacket(SerialManager::ReservedHostPortsEnum::RawSerial);
            else if (osData.serialManager->receiveBuffer->GetCount() > 0)
                return true;
            else
                return _CanRead();
        }
    }

    char Read()
    {
        if (osData.serialManager == NULL)
            return _Read();
        else
        {
            if (currentSerialReadPacket == NULL && osData.serialManager->clientConnected)
            {
                currentSerialReadPacket = osData.serialManager->GetPacket(SerialManager::ReservedHostPortsEnum::RawSerial);
                currentSerialReadPacketIndex = 0;
            }
            
            if (currentSerialReadPacket == NULL && osData.serialManager->clientConnected)
                return 0;
            if (currentSerialReadPacket != NULL)    
            {
                char ret = currentSerialReadPacket->data[currentSerialReadPacketIndex];
                currentSerialReadPacketIndex++;
                if (currentSerialReadPacketIndex >= currentSerialReadPacket->len)
                {
                    currentSerialReadPacket->Free();
                    currentSerialReadPacket = NULL;
                    currentSerialReadPacketIndex = 0;
                }
                
                return ret;
            }
            else if (osData.serialManager->receiveBuffer->GetCount() > 0)
            {
                char ret = osData.serialManager->receiveBuffer->ElementAt(0);
                osData.serialManager->receiveBuffer->RemoveAt(0);
                return ret;
            }
            else
                return _Read();
        }
    }
    bool CanWrite()
    {
        if (osData.serialManager == NULL ||
            !osData.serialManager->clientConnected)
            return _CanWrite();
        else
            return true;
    }
    void Write(char chr)
    {
        if (osData.serialManager == NULL ||
            !osData.serialManager->clientConnected)
            _Write(chr);
        else
        {
            SerialManager::GenericPacket* packet = 
            new SerialManager::GenericPacket(
                SerialManager::PacketType::DATA, 
                SerialManager::ReservedHostPortsEnum::RawSerial, 
                SerialManager::ReservedOutClientPortsEnum::RawSerialClient, 
                1, 
                (uint8_t*)&chr);
            osData.serialManager->SendPacket(packet);
        }
    }

    bool _CanRead()
    {
        return SerialWorks && (Sinb(5) & 1 == 1);
    }

    char _Read()
    {
        if (!SerialWorks)
            return 0;
        while (!_CanRead());
        return Sinb(0);
    }

    bool _CanWrite()
    {
        return SerialWorks && (Sinb(5) & 0x20);
    }

    void _Write(char chr)
    {
        if (!SerialWorks)
            return;
        while (!_CanWrite());
        Soutb(0, chr);
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
    void Writeln(const char *chrs, const char *var)
    {
        Writeln(chrs, var, true);
    }
    void Writeln(const char *chrs, const char *var, bool allowEscape)
    {
        Write(chrs, var, allowEscape);
        Writeln();
    }

    void Write(const char* str)
    {
        if (!SerialWorks || str == NULL)
            return;
        if (osData.serialManager == NULL ||
            !osData.serialManager->clientConnected)
        {
            while (*str != 0)
            {
                Write(*str);
                str++;
            }
            return;
        }
        else
        {
            SerialManager::GenericPacket* packet = 
            new SerialManager::GenericPacket(
                SerialManager::PacketType::DATA, 
                SerialManager::ReservedHostPortsEnum::RawSerial, 
                SerialManager::ReservedOutClientPortsEnum::RawSerialClient, 
                StrLen(str), 
                (uint8_t*)str);
            osData.serialManager->SendPacket(packet);
        }
    }

    void Write(const char *chrs, const char *var)
    {
        Write(chrs, var, true);
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

    void Soutb(uint16_t port, uint8_t value)
    {
        if (pciCard == 0)
            outb(SerialPort + port, value);
        else
            //PCI::write_byte(pciCard, port, value);
        {
            if (false)
                mOutb(pciIoBase + port, value);
            else
                outb(pciIoBase + port, value);
        }
    }

    uint8_t Sinb(uint16_t port)
    {
        if (pciCard == 0)
            return inb(SerialPort + port);
        else
            //return PCI::read_byte(pciCard, port);
        {
            if (false)
                return mInb(pciIoBase + port);
            else
                return inb(pciIoBase + port);
        }
    }
}