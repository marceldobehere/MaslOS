#pragma once
#include "stdint.h"
#include "../tasks/task.h"
#include "../OSDATA/MStack/MStackS.h"
#include "../WindowStuff/Window/window.h"
#include "../userinput/mousePacket.h"
#include "../WindowStuff/SubInstances/newTerminalInstance/consoleChar.h"


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
template class Array<ConsoleChar>;

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
    T elementAt(uint64_t index);
    T& operator[](uint64_t index);
    void clear();
    bool operator==(List<T> other);
};

template class List<int>;
template class List<Window*>;
template class List<Task*>;
template class List<MousePacket>;
template class List<ConsoleChar>;

template class Array<List<ConsoleChar>*>;

template class List<List<ConsoleChar>*>;

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