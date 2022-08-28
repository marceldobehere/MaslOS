#pragma once
#include "stdint.h"
//#include "array.h"
#define LIST_INCLUDE_WINDOW
#define LIST_INCLUDE_TASK
#include "list.h"
#undef LIST_INCLUDE_WINDOW
#undef LIST_INCLUDE_TASK


template <typename T> class SyncedList
{
    private:
    uint64_t count;
    uint64_t cap;
    Array<T> arr;
    List<T>* syncList;
    bool freed;
    void init(uint64_t capacity);
    void expandArr();
    void add(T item);
    void insertAt(T item, uint64_t index);
    void removeAt(uint64_t index);
    void removeLast();
    void removeFirst();

    public:
    //SyncedList<T>(uint64_t capacity);
    SyncedList<T>(List<T>* list);
    void free();
    //List<T> clone(); 
    uint64_t getCount();
    uint64_t getCapacity();
    int64_t getIndexOf(T item);

    void sync();
    void move(int64_t from, int64_t to);
    void move(T item, int64_t to);
    T& operator[](uint64_t index);
    T elementAt(uint64_t index);

};

template class SyncedList<int>;
template class SyncedList<Window*>;
template class SyncedList<Task*>;

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