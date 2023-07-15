#include "syncedList.h"
#include "../osData/MStack/MStackM.h"


template <typename T> void SyncedList<T>::init(uint64_t capacity)
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

template <typename T> SyncedList<T>::SyncedList(List<T>* list) : arr(Array<T>(0))
{
    AddToStack();
    syncList = list;
    init(list->getCapacity());
    for (int i = 0; i < list->getCount(); i++)
        add(list->elementAt(i));
    RemoveFromStack();
}



template <typename T> void SyncedList<T>::free()
{
    AddToStack();
    arr.free();
    cap = 0;
    freed = true;
    count = 0;
    RemoveFromStack();
}

template <typename T>void SyncedList<T>::expandArr()
{
    AddToStack();
    cap *= 2;
    Array<T> newArr = Array<T>(cap);
    arr.copy(newArr);
    arr.free();
    arr = newArr;
    RemoveFromStack();
}

// template <typename T> SyncedList<T> SyncedList<T>::clone()
// {
//     AddToStack();
//     List<T> newList = List<T>(cap);

//     for (uint64_t i = 0; i < count; i++)
//         newList.add(arr[i]);

//     RemoveFromStack();
//     return newList;
// }

template <typename T> uint64_t SyncedList<T>::getCount()
{
    return count;
}

template <typename T> uint64_t SyncedList<T>::getCapacity()
{
    return cap;
}


template <typename T> int64_t SyncedList<T>::getIndexOf(T item)
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

template <typename T> void SyncedList<T>::add(T item)
{
    AddToStack();
    if (count + 1 > cap)
        expandArr();
    
    arr[count] = item;
    count++;
    RemoveFromStack();
}

template <typename T> void SyncedList<T>::insertAt(T item, uint64_t index)
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

template <typename T> void SyncedList<T>::removeAt(uint64_t index)
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

template <typename T> void SyncedList<T>::removeFirst()
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

template <typename T> void SyncedList<T>::removeLast()
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

template <typename T> T& SyncedList<T>::operator[](uint64_t index)
{
    AddToStack();
    T& data = arr[index];
    RemoveFromStack();
    return data;
}

template <typename T> T SyncedList<T>::elementAt(uint64_t index)
{
    return arr[index];
}


template <typename T> void SyncedList<T>::sync()
{
    for (int i = 0; i < syncList->getCount(); i++)
        if (this->getIndexOf(syncList->elementAt(i)) == -1)
            add(syncList->elementAt(i));
        
    for (int i = 0; i < count; i++)
        if (syncList->getIndexOf(this->arr[i]) == -1)
        {
            removeAt(i);
            i--;
        }  
}

template <typename T> void SyncedList<T>::move(int64_t from, int64_t to)
{
    if (from < 0 || from >= count)
        return;
    if (to < 0 || to >= count)
        return;
    if (from == to)
        return;

    if (from < to)
        to--;

    T temp = arr[from];
    removeAt(from);
    insertAt(temp, to);
    

}

template <typename T> void SyncedList<T>::move(T item, int64_t to)
{
    move(getIndexOf(item), to);
}
