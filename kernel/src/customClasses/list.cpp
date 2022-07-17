#include "list.h"
#include "../OSDATA/MStack/MStackM.h"


template <typename T> void List<T>::init(uint64_t capacity)
{
    AddToStack();
    if (capacity < 2)
        capacity = 2;
    freed = false;
    count = 0;
    cap = capacity;
    arr.reInit(capacity);
    RemoveFromStack();
}

template <typename T> List<T>::List(uint64_t capacity) : arr(Array<T>(4))
{
    AddToStack();
    init(capacity);
    RemoveFromStack();
}

template <typename T> List<T>::List() : arr(Array<T>(4))
{
    AddToStack();
    init(4);
    RemoveFromStack();
}

template <typename T> void List<T>::free()
{
    AddToStack();
    arr.free();
    cap = 0;
    freed = true;
    count = 0;
    RemoveFromStack();
}

template <typename T>void List<T>::expandArr()
{
    AddToStack();
    cap *= 2;
    Array<T> newArr = Array<T>(cap);
    arr.copy(newArr);
    arr.free();
    arr = newArr;
    RemoveFromStack();
}

template <typename T> List<T> List<T>::clone()
{
    AddToStack();
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
    AddToStack();
    #if true
    for (uint64_t index = 0; index < count; index++)
        if (arr[index] == item)
        {
            RemoveFromStack();
            return index;
        }
    #endif
    RemoveFromStack();
    return -1;
}

template <typename T> void List<T>::add(T item)
{
    AddToStack();
    if (count + 1 > cap)
        expandArr();
    
    arr[count] = item;
    count++;
    RemoveFromStack();
}

template <typename T> void List<T>::insertAt(T item, uint64_t index)
{
    if (index > count)
        return;
    
    AddToStack();
    if (count + 1 > cap)
        expandArr();
    
    for (uint64_t i = count - 1; i >= index; i--)
        arr[i+1] = arr[i];
    
    arr[index] = item;

    count++;
    RemoveFromStack();
}

template <typename T> void List<T>::clear()
{
    AddToStack();
    // for (uint64_t i = 0; i < count; i++)
    //     arr[i] = *((T*)NULL);

    count = 0;
    RemoveFromStack();
}

template <typename T> void List<T>::removeAt(uint64_t index)
{
    AddToStack();
    if (index < 0 || index >= count)
    {
        RemoveFromStack();
        return;
    }
    for (uint64_t i = index + 1; i < count; i++)
    {
        arr[i - 1] = arr[i];
    }

    //arr[count] = *((T*)NULL);

    count--;
    RemoveFromStack();
}

template <typename T> void List<T>::removeFirst()
{
    AddToStack();
    if (count == 0)
    {
        RemoveFromStack();
        return;
    }

    removeAt(0);

    RemoveFromStack();
}

template <typename T> void List<T>::removeLast()
{
    AddToStack();
    if (count == 0)
    {
        RemoveFromStack();
        return;
    }

    //arr[count] = *((T*)NULL);

    count--;
    RemoveFromStack();
}

template <typename T> T& List<T>::operator[](uint64_t index)
{
    AddToStack();
    T& data = arr[index];
    RemoveFromStack();
    return data;
}

template <typename T> T List<T>::elementAt(uint64_t index)
{
    return arr[index];
}



template <typename T>bool List<T>::operator==(List<T> other)
{
    return false;
}









//---------------------------------------------ARRAYS---------------------------











template <typename T> Array<T>::Array(uint64_t size)
{
    AddToStack();
    this->size = size;
    arr = (T*)malloc(size * sizeof(T));
    freed = false;
    RemoveFromStack();
}

template <typename T> void Array<T>::reInit(uint64_t size)
{
    AddToStack();
    this->size = size;
    free();
    arr = (T*)malloc(size * sizeof(T));
    freed = false;
    RemoveFromStack();
}

template <typename T> T& Array<T>::operator[](uint64_t index)
{
    AddToStack();
    if (index >= this->size)
        Panic("Accessing Array out of bounds!");

    T& t = arr[index];
    RemoveFromStack();
    return t;
}

template <typename T> void Array<T>::free()
{
    AddToStack();
    if (freed)
    {
        RemoveFromStack();
        return;
    }

    if (arr != NULL)
        _free(arr);
    arr = NULL;
    freed = true;
    RemoveFromStack();
}


template <typename T> Array<T> Array<T>::clone()
{
    AddToStack();
    Array<T> newArr = Array<T>(size);

    for (uint64_t i = 0; i < size; i++)
        newArr[i] = arr[i];

    RemoveFromStack();
    return newArr;
}


template <typename T> void Array<T>::copy(Array<T> target)
{
    AddToStack();
    for (uint64_t i = 0; i < size; i++)
        target[i] = arr[i];
    RemoveFromStack();
}

template <typename T> uint64_t Array<T>::getSize()
{
    return size;
}