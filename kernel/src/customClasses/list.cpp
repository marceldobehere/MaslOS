#include "list.h"


template <typename T> void List<T>::init(uint64_t capacity)
{
    if (capacity < 2)
        capacity = 2;
    freed = false;
    count = 0;
    cap = capacity;
    arr.reInit(capacity);
}

template <typename T> List<T>::List(uint64_t capacity) : arr(Array<T>(0))
{
    init(capacity);
}

template <typename T> List<T>::List() : arr(Array<T>(0))
{
    init(4);
}

template <typename T> void List<T>::free()
{
    arr.free();
    cap = 0;
    freed = true;
    count = 0;
}

template <typename T>void List<T>::expandArr()
{
    cap *= 2;
    Array<T> newArr = Array<T>(cap);
    arr.copy(newArr);
    arr.free();
    arr = newArr;
}

template <typename T> List<T> List<T>::clone()
{
    List<T> newList = List<T>(cap);

    for (uint64_t i = 0; i < count; i++)
        newList.add(arr[i]);

    return newList;
}

template <typename T> uint64_t List<T>::getCount()
{
    return count;
}

template <typename T> uint64_t List<T>::getCapacity()
{
    return cap;
}

template <typename T> void List<T>::add(T item)
{
    if (count + 1 > cap)
        expandArr();
    
    arr[count] = item;
    count++;
}

template <typename T> void List<T>::clear()
{
    for (uint64_t i = 0; i < count; i++)
        arr[i] = T();

    count = 0;
}

template <typename T> void List<T>::removeAt(uint64_t index)
{
    if (index < 0 || index >= count)
        return;

    for (uint64_t i = index + 1; i < count; i++)
    {
        arr[i - 1] = arr[i];
    }
    arr[count] = T();

    count--;
}

template <typename T> T& List<T>::operator[](uint64_t index)
{
    return arr[index];
}