#include "list.h"
#include "../osData/MStack/MStackM.h"
#include "../memory/heap.h"
#include "./list/list_all.h"


template <typename T> void List<T>::Init(uint64_t capacity)
{
    AddToStack();
    if (capacity < 2)
        capacity = 2;
    freed = false;
    count = 0;
    cap = capacity;
    arr.ReInit(capacity);
    RemoveFromStack();
}

template <typename T> List<T>::List(uint64_t capacity) : arr(Array<T>(4))
{
    AddToStack();
    Init(capacity);
    RemoveFromStack();
}

template <typename T> List<T>::List() : arr(Array<T>(4))
{
    AddToStack();
    Init(4);
    RemoveFromStack();
}

template <typename T> void List<T>::Free()
{
    AddToStack();
    arr.Free();
    cap = 0;
    freed = true;
    count = 0;
    RemoveFromStack();
}

template <typename T>void List<T>::ExpandArr()
{
    AddToStack();
    Array<T> newArr = Array<T>(arr.GetSize());
    arr.Copy(newArr);
    
    cap *= 2;
    arr.ReInit(cap);
    
    newArr.Copy(arr);
    newArr.Free();
    RemoveFromStack();
}

template <typename T> List<T> List<T>::Clone()
{
    AddToStack();
    List<T> newList = List<T>(cap);

    for (uint64_t i = 0; i < count; i++)
        newList.Add(arr[i]);

    RemoveFromStack();
    return newList;
}

template <typename T> uint64_t List<T>::GetCount()
{
    return count;
}

template <typename T> uint64_t List<T>::GetCapacity()
{
    return cap;
}


template <typename T> int64_t List<T>::GetIndexOf(T item)
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

template <typename T> void List<T>::Add(T item)
{
    //AddToStack();
    if (count >= cap)
        ExpandArr();

    arr[count] = item;
    count++;
    //RemoveFromStack();
}

template <typename T> void List<T>::Set(uint64_t index, T item)
{
    //AddToStack();
    int64_t indx = index;
    if (index > count || indx < 0)
        return;
    arr[index] = item;
    
    //RemoveFromStack();
}

template <typename T> void List<T>::InsertAt(T item, uint64_t index)
{
    int64_t indx = index;
    if (index > count || indx < 0)
        return;
    
    AddToStack();
    if (count + 1 > cap)
        ExpandArr();
    


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

template <typename T> void List<T>::Clear()
{
    AddToStack();
    // for (uint64_t i = 0; i < count; i++)
    //     arr[i] = *((T*)NULL);

    count = 0;
    RemoveFromStack();
}

template <typename T> void List<T>::RemoveAt(uint64_t index)
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

template <typename T> void List<T>::RemoveFirst()
{
    AddToStack();
    if (count == 0)
    {
        RemoveFromStack();
        return;
    }

    RemoveAt(0);

    RemoveFromStack();
}

template <typename T> void List<T>::RemoveFirst(int amt)
{
    AddToStack();
    if (count == 0 || amt <= 0)
    {
        RemoveFromStack();
        return;
    }

    if (amt > count)
        amt = count;
    
    for (uint64_t i = amt; i < count; i++)
    {
        arr[i - amt] = arr[i];
    }
    count -= amt;

    RemoveFromStack();
}

template <typename T> void List<T>::RemoveLast()
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

template <typename T> T List<T>::ElementAt(uint64_t index)
{
    return arr[index];
}

template <typename T> T List<T>::LastElement()
{
    return arr[count - 1];
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

template <typename T> void Array<T>::ReInit(uint64_t size)
{
    AddToStack();
    if (size < 2)
        size = 2;
    this->size = size;


    Free();
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

template <typename T> void Array<T>::Free()
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


template <typename T> Array<T> Array<T>::Clone()
{
    AddToStack();
    Array<T> newArr = Array<T>(size);

    for (uint64_t i = 0; i < size; i++)
        newArr.arr[i] = arr[i];

    RemoveFromStack();
    return newArr;
}


template <typename T> void Array<T>::Copy(Array<T> target)
{
    AddToStack();
    for (uint64_t i = 0; i < size; i++)
        target.arr[i] = arr[i];
    RemoveFromStack();
}

template <typename T> uint64_t Array<T>::GetSize()
{
    return size;
}