#pragma once
#include <stdint.h>


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
    void insertAt(T item, uint64_t index);
    void removeAt(uint64_t index);
    void removeLast();
    void removeFirst();
    void removeFirst(int count);
    T elementAt(uint64_t index);
    T lastElement();
    void set(uint64_t index, T item);
    T& operator[](uint64_t index);
    void clear();
    bool operator==(List<T> other);
};





// #include "../kernelStuff/diskStuff/Partition_Interfaces/generic/genericPartitionInterface.h"

// // template class Array<PartitionInterface::GenericPartitionInterface*>;
// // template class  List<PartitionInterface::GenericPartitionInterface*>;

// #include "../kernelStuff/diskStuff/Filesystem_Interfaces/generic/genericFileSystemInterface.h"







