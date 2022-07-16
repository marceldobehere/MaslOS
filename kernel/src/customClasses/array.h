#pragma once
#include "stdint.h"
#include "../tasks/task.h"
#include "../OSDATA/MStack/MStackS.h"
#include "../WindowStuff/Window/window.h"
#include "../userinput/mousePacket.h"


template <typename T> class Array
{
    private:
    T* arr;
    bool freed;
    uint64_t size;

    public:
    //bool freed;
    uint64_t getSize();
    Array<T>(uint64_t size);
    T& operator[](uint64_t index);
    void free();
    Array<T> clone();
    void copy(Array<T> target);
    void reInit(uint64_t size);
};

template class Array<int>;
template class Array<Window*>;
template class Array<Task*>;
template class Array<MousePacket>;