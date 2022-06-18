#include "list.h"
#include "../OSDATA/MStack/MStackM.h"


template <typename T> void List<T>::init(uint64_t capacity)
{
    AddToStack("init", "customClasses/list.cpp");
    if (capacity < 2)
        capacity = 2;
    freed = false;
    count = 0;
    cap = capacity;
    arr.reInit(capacity);
    RemoveFromStack();
}

template <typename T> List<T>::List(uint64_t capacity) : arr(Array<T>(0))
{
    AddToStack("Constructor", "customClasses/list.cpp");
    init(capacity);
    RemoveFromStack();
}

template <typename T> List<T>::List() : arr(Array<T>(0))
{
    AddToStack("Constructor", "customClasses/list.cpp");
    init(4);
    RemoveFromStack();
}

template <typename T> void List<T>::free()
{
    AddToStack("free", "customClasses/list.cpp");
    arr.free();
    cap = 0;
    freed = true;
    count = 0;
    RemoveFromStack();
}

template <typename T>void List<T>::expandArr()
{
    AddToStack("expandArr", "customClasses/list.cpp");
    cap *= 2;
    Array<T> newArr = Array<T>(cap);
    arr.copy(newArr);
    arr.free();
    arr = newArr;
    RemoveFromStack();
}

template <typename T> List<T> List<T>::clone()
{
    AddToStack("clone", "customClasses/list.cpp");
    List<T> newList = List<T>(cap);

    for (uint64_t i = 0; i < count; i++)
        newList.add(arr[i]);

    RemoveFromStack();
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


template <typename T> int64_t List<T>::getIndexOf(T item)
{
    AddToStack("getIndexOf", "customClasses/list.cpp");
    for (uint64_t index = 0; index < count; index++)
        if (arr[index] == item)
        {
            RemoveFromStack();
            return index;
        }
    RemoveFromStack();
    return -1;
}

template <typename T> void List<T>::add(T item)
{
    AddToStack("add", "customClasses/list.cpp");
    if (count + 1 > cap)
        expandArr();
    
    arr[count] = item;
    count++;
    RemoveFromStack();
}

template <typename T> void List<T>::clear()
{
    AddToStack("clear", "customClasses/list.cpp");
    for (uint64_t i = 0; i < count; i++)
        arr[i] = *((T*)NULL);

    count = 0;
    RemoveFromStack();
}

template <typename T> void List<T>::removeAt(uint64_t index)
{
    AddToStack("removeAt", "customClasses/list.cpp");
    if (index < 0 || index >= count)
    {
        RemoveFromStack();
        return;
    }

    for (uint64_t i = index + 1; i < count; i++)
    {
        arr[i - 1] = arr[i];
    }
    arr[count] = *((T*)NULL);

    count--;
    RemoveFromStack();
}

template <typename T> void List<T>::removeFirst()
{
    AddToStack("removeFirst", "customClasses/list.cpp");
    if (count == 0)
        return;

    removeAt(0);

    RemoveFromStack();
}

template <typename T> void List<T>::removeLast()
{
    AddToStack("removeLast", "customClasses/list.cpp");
    if (count == 0)
        return;

    arr[count] = *((T*)NULL);

    count--;
    RemoveFromStack();
}

template <typename T> T& List<T>::operator[](uint64_t index)
{
    AddToStack("operator[]", "customClasses/list.cpp");
    T& data = arr[index];
    RemoveFromStack();
    return data;
}