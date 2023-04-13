#pragma once
#include <stdint.h>


template <typename T> class Array
{
    private:
    T* arr;
    bool freed;
    uint64_t size;

    public:
    //bool freed;
    uint64_t getSize();
    Array<T>(uint64_t size);
    T& operator[](uint64_t index);
    void free();
    Array<T> clone();
    void copy(Array<T> target);
    void reInit(uint64_t size);
};

template <typename T> class List
{
    private:
    uint64_t count;
    uint64_t cap;
    Array<T> arr;
    bool freed;
    void init(uint64_t capacity);
    void expandArr();

    public:
    List<T>(uint64_t capacity);
    List<T>();
    void free();
    List<T> clone(); 
    uint64_t getCount();
    uint64_t getCapacity();
    int64_t getIndexOf(T item);
    void add(T item);
    void insertAt(T item, uint64_t index);
    void removeAt(uint64_t index);
    void removeLast();
    void removeFirst();
    void removeFirst(int count);
    T elementAt(uint64_t index);
    void set(uint64_t index, T item);
    T& operator[](uint64_t index);
    void clear();
    bool operator==(List<T> other);
};

template class Array<int>;
template class List<int>;

template class Array<bool>;
template class List<bool>;

template class Array<uint8_t>;
template class List<uint8_t>;

template class Array<const char*>;
template class List<const char*>;

template class Array<void*>;
template class List<void*>;

#include "../tasks/task.h"

template class Array<Task*>;
template class List<Task*>;

#include "../WindowStuff/Window/window.h"

template class Array<Window*>;
template class List<Window*>;

#include "../userinput/mousePacket.h"

template class Array<MousePacket>;
template class List<MousePacket>;

#include "../WindowStuff/SubInstances/newTerminalInstance/consoleChar.h"

template class Array<ConsoleChar>;
template class List<ConsoleChar>;
template class Array<List<ConsoleChar>*>;
template class List<List<ConsoleChar>*>;

#include "../kernelStuff/Disk_Stuff/Partition_Interfaces/generic/partitionStructs.h"

template class Array<PartitionInterface::PartitionInfo*>;
template class  List<PartitionInterface::PartitionInfo*>;

#include "../kernelStuff/Disk_Stuff/Filesystem_Interfaces/generic/fileSystemStructs.h"

template class Array<FilesystemInterface::BaseInfo*>;
template class  List<FilesystemInterface::BaseInfo*>;

template class Array<FilesystemInterface::FileInfo*>;
template class  List<FilesystemInterface::FileInfo*>;

template class Array<FilesystemInterface::FolderInfo*>;
template class  List<FilesystemInterface::FolderInfo*>;

// #include "../kernelStuff/Disk_Stuff/Partition_Interfaces/generic/genericPartitionInterface.h"

// template class Array<PartitionInterface::GenericPartitionInterface*>;
// template class  List<PartitionInterface::GenericPartitionInterface*>;

#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/generic/genericDiskInterface.h"

template class Array<DiskInterface::GenericDiskInterface*>;
template class List<DiskInterface::GenericDiskInterface*>;



#include "../WindowStuff/SubInstances/guiInstance/guiStuff/generalStuff.h"

template class Array<GuiComponentStuff::Field>;
template class List<GuiComponentStuff::Field>;

#include "../musicTest/musicStruct.h"

template class Array<Music::Note>;
template class List<Music::Note>;

template class Array<Music::NoteCommand>;
template class List<Music::NoteCommand>;































































/*

//#pragma once
#include "testListThing.h"


#ifdef  LIST_INCLUDE_TASK
#ifndef LIST_INCLUDE_TASK_DONE
#define LIST_INCLUDE_TASK_DONE
#include "../tasks/task.h"

template class Array<Task*>;
template class List<Task*>;
#endif
#endif


#ifdef  LIST_INCLUDE_PARTITION_INFO
#ifndef LIST_INCLUDE_PARTITION_INFO_DONE
#define LIST_INCLUDE_PARTITION_INFO_DONE
#include "../kernelStuff/Disk_Stuff/Partition_Interfaces/generic/genericPartitionInterface.h"

template class Array<PartitionInterface::PartitionInfo*>;
template class  List<PartitionInterface::PartitionInfo*>;
#endif
#endif


#ifdef  LIST_INCLUDE_WINDOW
#ifndef LIST_INCLUDE_WINDOW_DONE
#define LIST_INCLUDE_WINDOW_DONE
#include "../WindowStuff/Window/window.h"

template class Array<Window*>;
template class List<Window*>;
#endif
#endif

#ifdef  LIST_INCLUDE_MOUSE_PACKET
#ifndef LIST_INCLUDE_MOUSE_PACKET_DONE
#define LIST_INCLUDE_MOUSE_PACKET_DONE
#include "../userinput/mousePacket.h"

template class Array<MousePacket>;
template class List<MousePacket>;
#endif
#endif

#ifdef  LIST_INCLUDE_CONSOLE_CHAR
#ifndef LIST_INCLUDE_CONSOLE_CHAR_DONE
#define LIST_INCLUDE_CONSOLE_CHAR_DONE
#include "../WindowStuff/SubInstances/newTerminalInstance/consoleChar.h"

template class Array<ConsoleChar>;
template class List<ConsoleChar>;
template class Array<List<ConsoleChar>*>;
template class List<List<ConsoleChar>*>;
#endif
#endif

#ifdef  LIST_INCLUDE_GENERIC_DISK_INTERFACE
#ifndef LIST_INCLUDE_GENERIC_DISK_INTERFACE_DONE
#define LIST_INCLUDE_GENERIC_DISK_INTERFACE_DONE
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/generic/genericDiskInterface.h"

template class Array<DiskInterface::GenericDiskInterface*>;
template class List<DiskInterface::GenericDiskInterface*>;
#endif
#endif

// #ifdef  LIST_INCLUDE_PARTITION_INFO
// #ifndef LIST_INCLUDE_PARTITION_INFO_DONE
// #define LIST_INCLUDE_PARTITION_INFO_DONE
// // #include "../kernelStuff/Disk_Stuff/Partition_Interfaces/generic/partitionStructs.h"
// #include "../kernelStuff/Disk_Stuff/Partition_Interfaces/generic/bruh.h"

// template class Array<BRUH*>;
// template class List<BRUH*>;
// #endif
// #endif


#ifdef  LIST_INCLUDE_GENERIC_PARTITION_INTERFACE
#ifndef LIST_INCLUDE_GENERIC_PARTITION_INTERFACE_DONE
#define LIST_INCLUDE_GENERIC_PARTITION_INTERFACE_DONE
#include "../kernelStuff/Disk_Stuff/Partition_Interfaces/generic/genericPartitionInterface.h"

template class Array<PartitionInterface::GenericPartitionInterface*>;
template class List<PartitionInterface::GenericPartitionInterface*>;
#endif
#endif



*/

//#include "../OSDATA/MStack/MStackS.h"







/*
template <typename T> class Array
{
    private:
    T* arr;
    T def = T();
    //bool freed;

    public:
    bool freed;
    const uint64_t size;
    Array<T>(uint64_t size);
    T& operator[](uint64_t index);
    void free();
    Array clone();


};

template class Array<int>;
*/



/*

#pragma once
#include "stdint.h"
#include "../tasks/task.h"
#include "../OSDATA/MStack/MStackS.h"
#include "../WindowStuff/Window/window.h"
#include "../userinput/mousePacket.h"
#include "../WindowStuff/SubInstances/newTerminalInstance/consoleChar.h"
#include "../kernelStuff/Disk_Stuff/Disk_Interfaces/generic/genericDiskInterface.h"

template <typename T> class Array
{
    private:
    T* arr;
    bool freed;
    uint64_t size;

    public:
    //bool freed;
    uint64_t getSize();
    Array<T>(uint64_t size);
    T& operator[](uint64_t index);
    void free();
    Array<T> clone();
    void copy(Array<T> target);
    void reInit(uint64_t size);
};

template class Array<int>;
template class Array<Window*>;
template class Array<Task*>;
template class Array<MousePacket>;
template class Array<ConsoleChar>;
template class Array<DiskInterface::GenericDiskInterface*>;

template <typename T> class List
{
    private:
    uint64_t count;
    uint64_t cap;
    Array<T> arr;
    bool freed;
    void init(uint64_t capacity);
    void expandArr();

    public:
    List<T>(uint64_t capacity);
    List<T>();
    void free();
    List<T> clone(); 
    uint64_t getCount();
    uint64_t getCapacity();
    int64_t getIndexOf(T item);
    void add(T item);
    void insertAt(T item, uint64_t index);
    void removeAt(uint64_t index);
    void removeLast();
    void removeFirst();
    T elementAt(uint64_t index);
    T& operator[](uint64_t index);
    void clear();
    bool operator==(List<T> other);
};

template class List<int>;
template class List<Window*>;
template class List<Task*>;
template class List<MousePacket>;
template class List<ConsoleChar>;
template class List<DiskInterface::GenericDiskInterface*>;

template class Array<List<ConsoleChar>*>;

template class List<List<ConsoleChar>*>;

*/