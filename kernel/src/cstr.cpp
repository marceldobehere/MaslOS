#include "cstr.h"

char intTo_stringOutput[128];
const char* to_string(uint64_t value)
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

const char* to_string(int64_t value)
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

const char* to_string(int value)
{
    return to_string((int64_t) value);
}


char doubleTo_stringOutput[128];
const char* to_string(double value, uint8_t places)
{
    uint8_t size = 0;
    if (value < 0)
    {
        value *= -1;
        doubleTo_stringOutput[0] = '-';
        size = 1;
    }


    uint64_t sizetest = (int64_t)value;
    while (sizetest / 10 > 0)
    {
        sizetest /= 10;
        size++;
    }

    uint8_t index = 0;

    if(places > 0)
    {
        size += places + 1;


        uint64_t temp = 1;

        for (int i = 0; i < places; i++)
            temp *= 10;

        uint64_t value3 = (uint64_t)((value - ((uint64_t)value))*temp); 

        while (value3 > 0)
        {        
            uint8_t remainder = value3 % 10;
            value3 /= 10;
            doubleTo_stringOutput[size - index] = remainder + '0';
            index++;
        }

        doubleTo_stringOutput[size - index] = '.';
        index++;
    }
    


    uint64_t value2 = (int64_t)value;
    while (value2 > 0)
    {        
        uint8_t remainder = value2 % 10;
        value2 /= 10;
        doubleTo_stringOutput[size - index] = remainder + '0';
        index++;
    }


    

    doubleTo_stringOutput[size + 1] = 0;


    return doubleTo_stringOutput;
}

const char* to_string(double value)
{
    return to_string(value, 2);
}

const char* to_string(bool value)
{
    if (value)
        return "true";
    else
        return "false";
}



unsigned int ConvertStringToHex(const char* data)
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


char hexTo_stringOutput[128];
const char* hexABC = "0123456789ABCDEF";
const char* ConvertHexToString(uint64_t hex, uint8_t size)
{
    hex = (hex << (16 - size) * 4);
    for (uint8_t i = 0; i < size; i++)
        hexTo_stringOutput[i] = hexABC[((hex << i * 4) >> ((15) * 4))];
    
    hexTo_stringOutput[size] = 0;

    return hexTo_stringOutput; 
}
const char* ConvertHexToString(uint64_t hex)
{
    return ConvertHexToString(hex, (64/4));
}
const char* ConvertHexToString(uint32_t hex)
{
    return ConvertHexToString(hex, (32/4));
}
const char* ConvertHexToString(uint16_t hex)
{
    return ConvertHexToString(hex, (16/4));
}
const char* ConvertHexToString(uint8_t hex)
{
    return ConvertHexToString(hex, (8/4));
}

int64_t to_int(const char* string)
{
    uint64_t number = 0;
    uint64_t size = 0;
    while (string[size] != 0)
        size++;

    int64_t end = 0;
    int64_t exp = 1;
    if (string[0] == '-')
    {
        end = 1;
        exp *= -1;
    }

    for (int64_t i = size - 1; i >= end; i--)
    {
        number += exp * (string[i] - '0');
        exp *= 10;
    }
    return number;
}