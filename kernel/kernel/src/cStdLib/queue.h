#pragma once
#include <stddef.h>
#include <stdint.h>

template <typename T> class Queue
{
    private:
    T* arr;
    bool freed;
    int64_t arrSize;
    int64_t count; // count
    int64_t head;
    int64_t tail;
    void Resize(uint64_t newSize);

    public:
    //bool freed;
    int64_t GetCount();
    Queue<T>(int64_t size);
    Queue<T>();
    T Dequeue();
    void Enqueue(T item);
    void Clear();
    void Free();
};