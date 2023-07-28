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
    uint64_t GetSize();
    Array<T>(uint64_t size);
    T& operator[](uint64_t index);
    void Free();
    Array<T> Clone();
    void Copy(Array<T> target);
    void ReInit(uint64_t size);
};

template <typename T> class List
{
    private:
    uint64_t count;
    uint64_t cap;
    Array<T> arr;
    bool freed;
    void Init(uint64_t capacity);
    void ExpandArr();

    public:
    List<T>(uint64_t capacity);
    List<T>();
    void Free();
    List<T> Clone(); 
    uint64_t GetCount();
    uint64_t GetCapacity();
    int64_t GetIndexOf(T item);
    void Add(T item);
    void InsertAt(T item, uint64_t index);
    void RemoveAt(uint64_t index);
    void RemoveLast();
    void RemoveFirst();
    void RemoveFirst(int count);
    T ElementAt(uint64_t index);
    T LastElement();
    void Set(uint64_t index, T item);
    T& operator[](uint64_t index);
    void Clear();
    bool operator==(List<T> other);
};





// #include "../kernelStuff/diskStuff/Partition_Interfaces/generic/genericPartitionInterface.h"

// // template class Array<PartitionInterface::GenericPartitionInterface*>;
// // template class  List<PartitionInterface::GenericPartitionInterface*>;

// #include "../kernelStuff/diskStuff/Filesystem_Interfaces/generic/genericFileSystemInterface.h"








