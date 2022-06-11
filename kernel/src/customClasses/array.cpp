#include "array.h"
#include "../memory/heap.h"

template <typename T> Array<T>::Array(uint64_t size)
{
    this->size = size;
    arr = (T*)malloc(size * sizeof(T));
    freed = false;
}

template <typename T> void Array<T>::reInit(uint64_t size)
{
    this->size = size;
    free();
    arr = (T*)malloc(size * sizeof(T));
    freed = false;
}

template <typename T> T& Array<T>::operator[](uint64_t index)
{
    if (index >= this->size)
        return this->def;

    return arr[index];
}

template <typename T> void Array<T>::free()
{
    if (freed)
        return;
    
    if (arr != NULL)
        _free(arr);
    arr = NULL;
    freed = true;
}


template <typename T> Array<T> Array<T>::clone()
{
    Array<T> newArr = Array<T>(size);

    for (uint64_t i = 0; i < size; i++)
        newArr[i] = arr[i];

    return newArr;
}


template <typename T> void Array<T>::copy(Array<T> target)
{
    for (uint64_t i = 0; i < size; i++)
        target[i] = arr[i];
}

template <typename T> uint64_t Array<T>::getSize()
{
    return size;
}