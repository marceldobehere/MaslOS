#include "list.h"
#include "../OSDATA/MStack/MStackM.h"


template <typename T> void List<T>::init(uint64_t capacity)
{
    AddToMStack(MStack("init", "customClasses/list.cpp"));
    if (capacity < 2)
        capacity = 2;
    freed = false;
    count = 0;
    cap = capacity;
    arr.reInit(capacity);
    RemoveLastMStack();
}

template <typename T> List<T>::List(uint64_t capacity) : arr(Array<T>(0))
{
    AddToMStack(MStack("Constructor", "customClasses/list.cpp"));
    init(capacity);
    RemoveLastMStack();
}

template <typename T> List<T>::List() : arr(Array<T>(0))
{
    AddToMStack(MStack("Constructor", "customClasses/list.cpp"));
    init(4);
    RemoveLastMStack();
}

template <typename T> void List<T>::free()
{
    AddToMStack(MStack("free", "customClasses/list.cpp"));
    arr.free();
    cap = 0;
    freed = true;
    count = 0;
    RemoveLastMStack();
}

template <typename T>void List<T>::expandArr()
{
    AddToMStack(MStack("expandArr", "customClasses/list.cpp"));
    cap *= 2;
    Array<T> newArr = Array<T>(cap);
    arr.copy(newArr);
    arr.free();
    arr = newArr;
    RemoveLastMStack();
}

template <typename T> List<T> List<T>::clone()
{
    AddToMStack(MStack("clone", "customClasses/list.cpp"));
    List<T> newList = List<T>(cap);

    for (uint64_t i = 0; i < count; i++)
        newList.add(arr[i]);

    RemoveLastMStack();
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
    AddToMStack(MStack("getIndexOf", "customClasses/list.cpp"));
    for (uint64_t index = 0; index < count; index++)
        if (arr[index] == item)
        {
            RemoveLastMStack();
            return index;
        }
    RemoveLastMStack();
    return -1;
}

template <typename T> void List<T>::add(T item)
{
    AddToMStack(MStack("add", "customClasses/list.cpp"));
    if (count + 1 > cap)
        expandArr();
    
    arr[count] = item;
    count++;
    RemoveLastMStack();
}

template <typename T> void List<T>::clear()
{
    AddToMStack(MStack("clear", "customClasses/list.cpp"));
    for (uint64_t i = 0; i < count; i++)
        arr[i] = *((T*)NULL);

    count = 0;
    RemoveLastMStack();
}

template <typename T> void List<T>::removeAt(uint64_t index)
{
    AddToMStack(MStack("removeAt", "customClasses/list.cpp"));
    if (index < 0 || index >= count)
    {
        RemoveLastMStack();
        return;
    }

    for (uint64_t i = index + 1; i < count; i++)
    {
        arr[i - 1] = arr[i];
    }
    arr[count] = *((T*)NULL);

    count--;
    RemoveLastMStack();
}

template <typename T> void List<T>::removeFirst()
{
    AddToMStack(MStack("removeFirst", "customClasses/list.cpp"));
    if (count == 0)
        return;

    removeAt(0);

    count--;
    RemoveLastMStack();
}

template <typename T> void List<T>::removeLast()
{
    AddToMStack(MStack("removeLast", "customClasses/list.cpp"));
    if (count == 0)
        return;

    arr[count] = *((T*)NULL);

    count--;
    RemoveLastMStack();
}

template <typename T> T& List<T>::operator[](uint64_t index)
{
    AddToMStack(MStack("operator[]", "customClasses/list.cpp"));
    T& data = arr[index];
    RemoveLastMStack();
    return data;
}