#pragma once
#include "stdint.h"
#include "array.h"


template <typename T> class List
{
    private:
    uint64_t count;
    uint64_t cap;
    Array<T> arr;
    bool freed;
    void init(uint64_t capacity);
    void expandArr();

    public:
    List<T>(uint64_t capacity);
    List<T>();
    void free();
    List<T> clone(); 
    uint64_t getCount();
    uint64_t getCapacity();
    int64_t getIndexOf(T item);
    void add(T item);
    void removeAt(uint64_t index);
    T& operator[](uint64_t index);
    void clear();
};

template class List<int>;
template class List<Window*>;

/*
template <typename T> class Array
{
    private:
    T* arr;
    T def = T();
    //bool freed;

    public:
    bool freed;
    const uint64_t size;
    Array<T>(uint64_t size);
    T& operator[](uint64_t index);
    void free();
    Array clone();


};

template class Array<int>;
*/