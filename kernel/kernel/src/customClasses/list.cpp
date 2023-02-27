#include "list.h"
#include "../OSDATA/MStack/MStackM.h"
#include "../memory/heap.h"


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
    Array<T> newArr = Array<T>(arr.getSize());
    arr.copy(newArr);
    
    cap *= 2;
    arr.reInit(cap);
    
    newArr.copy(arr);
    newArr.free();
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
    AddToStack();
    if (count >= cap)
        expandArr();

    arr[count] = item;
    count++;
    RemoveFromStack();
}

template <typename T> void List<T>::insertAt(T item, uint64_t index)
{
    int64_t indx = index;
    if (index > count || indx < 0)
        return;
    
    AddToStack();
    if (count + 1 > cap)
        expandArr();
    


    AddToStack();
    for (int64_t i = count - 1; i >= indx; i--)
    {
        arr[i+1] = arr[i];
    }
    RemoveFromStack();
    
    arr[indx] = item;

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
    //AddToStack();
    T& data = arr[index];
    //RemoveFromStack();
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
    if (size < 2)
        size = 2;
    this->size = size;
    arr = (T*)_Malloc(size * sizeof(T), "Array Constructor");
    freed = false;
    RemoveFromStack();
}

template <typename T> void Array<T>::reInit(uint64_t size)
{
    AddToStack();
    if (size < 2)
        size = 2;
    this->size = size;


    free();
    arr = (T*)_Malloc(size * sizeof(T), "Array re-init");

    freed = false;
    RemoveFromStack();
}

template <typename T> T& Array<T>::operator[](uint64_t index)
{
    //AddToStack();
    if (index >= this->size || freed)
    {
        AddToStack();
        Panic("Accessing Array out of bounds!", true);
        RemoveFromStack();
    }

    T& t = arr[index];
    //RemoveFromStack();
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
        _Free(arr);
    arr = NULL;
    freed = true;
    RemoveFromStack();
}


template <typename T> Array<T> Array<T>::clone()
{
    AddToStack();
    Array<T> newArr = Array<T>(size);

    for (uint64_t i = 0; i < size; i++)
        newArr.arr[i] = arr[i];

    RemoveFromStack();
    return newArr;
}


template <typename T> void Array<T>::copy(Array<T> target)
{
    AddToStack();
    for (uint64_t i = 0; i < size; i++)
        target.arr[i] = arr[i];
    RemoveFromStack();
}

template <typename T> uint64_t Array<T>::getSize()
{
    return size;
}