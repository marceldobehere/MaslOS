#pragma once
#include <stddef.h>
#include <stdint.h>

template <typename T> class Queue
{
    private:
    T* arr;
    bool freed;
    uint64_t arrSize;
    uint64_t count; // count
    uint64_t head;
    uint64_t tail;
    void Resize(uint64_t newSize);

    public:
    //bool freed;
    uint64_t GetCount();
    Queue<T>(uint64_t size);
    Queue<T>();
    T Dequeue();
    void Enqueue(T item);
    void Clear();
    void Free();
};