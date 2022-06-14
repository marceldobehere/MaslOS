#include "array.h"
#include "../memory/heap.h"
#include "../OSDATA/MStack/MStackM.h"

template <typename T> Array<T>::Array(uint64_t size)
{
    AddToMStack(MStack("Constructor", "customClasses/array.cpp"));
    this->size = size;
    arr = (T*)malloc(size * sizeof(T));
    freed = false;
    RemoveLastMStack();
}

template <typename T> void Array<T>::reInit(uint64_t size)
{
    AddToMStack(MStack("reInit", "customClasses/array.cpp"));
    this->size = size;
    free();
    arr = (T*)malloc(size * sizeof(T));
    freed = false;
    RemoveLastMStack();
}

template <typename T> T& Array<T>::operator[](uint64_t index)
{
    AddToMStack(MStack("operator[]", "customClasses/array.cpp"));
    if (index >= this->size)
        return *((T*)NULL);

    T& t = arr[index];
    RemoveLastMStack();
    return t;
}

template <typename T> void Array<T>::free()
{
    AddToMStack(MStack("free", "customClasses/array.cpp"));
    if (freed)
    {
        RemoveLastMStack();
        return;
    }

    if (arr != NULL)
        _free(arr);
    arr = NULL;
    freed = true;
    RemoveLastMStack();
}


template <typename T> Array<T> Array<T>::clone()
{
    AddToMStack(MStack("clone", "customClasses/array.cpp"));
    Array<T> newArr = Array<T>(size);

    for (uint64_t i = 0; i < size; i++)
        newArr[i] = arr[i];

    RemoveLastMStack();
    return newArr;
}


template <typename T> void Array<T>::copy(Array<T> target)
{
    AddToMStack(MStack("copy", "customClasses/array.cpp"));
    for (uint64_t i = 0; i < size; i++)
        target[i] = arr[i];
    RemoveLastMStack();
}

template <typename T> uint64_t Array<T>::getSize()
{
    return size;
}