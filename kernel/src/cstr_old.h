#pragma once

#include <stdint.h>

static char intTo_stringOutput[128];
static const char* to_string(uint64_t value)
{
    uint8_t size = 0;
    uint64_t sizetest = value;
    while (sizetest / 10 > 0)
    {
        sizetest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {        
        uint8_t remainder = value % 10;
        value /= 10;
        intTo_stringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intTo_stringOutput[size - index] = remainder + '0';
    intTo_stringOutput[size + 1] = 0;
    return intTo_stringOutput;
}

static const char* to_string(int64_t value)
{
    uint8_t size = 0;
    if (value < 0)
    {
        value *= -1;
        intTo_stringOutput[0] = '-';
        size = 1;
    }


    uint64_t sizetest = value;
    while (sizetest / 10 > 0)
    {
        sizetest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {        
        uint8_t remainder = value % 10;
        value /= 10;
        intTo_stringOutput[size - index] = remainder + '0';
        index++;
    }

    
    uint8_t remainder = value % 10;
    intTo_stringOutput[size - index] = remainder + '0';
    intTo_stringOutput[size + 1] = 0;


    return intTo_stringOutput;
}



static unsigned int ConvertStringToHex(const char* data)
{
    unsigned int hex = 0;

    for (unsigned int i = 0; i < 6;)
    {
        unsigned char temp = 0;
        for (unsigned int i2 = 16; i2 != 0; i2 /= 16)
        {
            if (data[i] >= '0' && data[i] <= '9')
                temp += (data[i] - '0') * i2;
            else if (data[i] >= 'A' && data[i] <= 'F')
                temp += (data[i] + 10 - 'A') * i2;
            else if (data[i] >= 'a' && data[i] <= 'f')
                temp += (data[i] + 10 - 'a') * i2;
            i++;
        }
        hex = hex << 8;
        hex += temp;
    }
    
    return hex; 
}


static char hexTo_stringOutput[128];
static const char* hexABC = "0123456789ABCDEF";
static const char* ConvertHexToString(uint64_t hex, uint8_t size)
{
    hex = (hex << (16 - size) * 4);
    for (uint8_t i = 0; i < size; i++)
        hexTo_stringOutput[i] = hexABC[((hex << i * 4) >> ((15) * 4))];
    
    hexTo_stringOutput[size] = 0;

    return hexTo_stringOutput; 
}
static const char* ConvertHexToString(uint64_t hex)
{
    return ConvertHexToString(hex, (64/4));
}
static const char* ConvertHexToString(uint32_t hex)
{
    return ConvertHexToString(hex, (32/4));
}
static const char* ConvertHexToString(uint16_t hex)
{
    return ConvertHexToString(hex, (16/4));
}
static const char* ConvertHexToString(uint8_t hex)
{
    return ConvertHexToString(hex, (8/4));
}