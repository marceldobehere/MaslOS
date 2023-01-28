#pragma once
#include <stdint.h>

struct MousePacket
{
    uint8_t data[4];

    MousePacket(uint8_t data[4])
    {
        this->data[0] = data[0];
        this->data[1] = data[1];
        this->data[2] = data[2];
        this->data[3] = data[3];
    }

    bool operator==(const MousePacket& other)
    {
        return  this->data[0] == other.data[0] &&
                this->data[1] == other.data[1] && 
                this->data[2] == other.data[2] && 
                this->data[3] == other.data[3];
    }
};