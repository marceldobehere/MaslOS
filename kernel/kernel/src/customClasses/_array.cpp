// #include "_array.h"
// #include "../memory/heap.h"
// #include "../OSDATA/MStack/MStackM.h"

// template <typename T> Array<T>::Array(uint64_t size)
// {
//     AddToStack();
//     this->size = size;
//     arr = (T*)malloc(size * sizeof(T));
//     freed = false;
//     RemoveFromStack();
// }

// template <typename T> void Array<T>::reInit(uint64_t size)
// {
//     AddToStack();
//     this->size = size;
//     free();
//     arr = (T*)malloc(size * sizeof(T));
//     freed = false;
//     RemoveFromStack();
// }

// template <typename T> T& Array<T>::operator[](uint64_t index)
// {
//     AddToStack();
//     if (index >= this->size)
//         Panic("Accessing Array out of bounds!");

//     T& t = arr[index];
//     RemoveFromStack();
//     return t;
// }

// template <typename T> void Array<T>::free()
// {
//     AddToStack();
//     if (freed)
//     {
//         RemoveFromStack();
//         return;
//     }

//     if (arr != NULL)
//         _free(arr);
//     arr = NULL;
//     freed = true;
//     RemoveFromStack();
// }


// template <typename T> Array<T> Array<T>::clone()
// {
//     AddToStack();
//     Array<T> newArr = Array<T>(size);

//     for (uint64_t i = 0; i < size; i++)
//         newArr[i] = arr[i];

//     RemoveFromStack();
//     return newArr;
// }


// template <typename T> void Array<T>::copy(Array<T> target)
// {
//     AddToStack();
//     for (uint64_t i = 0; i < size; i++)
//         target[i] = arr[i];
//     RemoveFromStack();
// }

// template <typename T> uint64_t Array<T>::getSize()
// {
//     return size;
// }