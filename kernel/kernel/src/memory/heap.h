#pragma once

#include <stdint.h>
#include <stddef.h>


struct HeapSegHdr
{
    size_t length;
    HeapSegHdr* next;
    HeapSegHdr* last;
    const char* text = "<DEF>";
    int64_t activeMemFlagVal = 0;
    const char* file = "<DEF>";
    const char* func = "<DEF>";
    int line = 0;
    uint64_t time = 0;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr* Split(size_t splitLength);
    uint32_t magicNum;
};


void InitializeHeap(void* heapAddress, size_t pageCount);

void* _Xmalloc(size_t size, const char* func, const char* file, int line);
void _Xfree(void* address, const char* func, const char* file, int line);
bool _XtryFree(void* address, const char* func, const char* file, int line);

// void* _X_malloc(size_t size, const char* func, const char* file, int line);
// void _X_free(void* address, const char* func, const char* file, int line);

void* _Xmalloc(size_t size, const char* text, const char* func, const char* file, int line);
// void* _X_malloc(size_t size, const char* text, const char* func, const char* file, int line);


bool HeapCheck(bool wait);



// get number of arguments with __NARG__
#define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
#define __NARG_I_(...) __ARG_N(__VA_ARGS__)
#define __ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

// definition for FOO
#define _Malloc(...) VFUNC(_Malloc, __VA_ARGS__)





#define _Malloc1(size) _Xmalloc(size, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _Malloc2(size, text) _Xmalloc(size, text, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _Free(address) _Xfree((void*)address, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _TryFree(address) _XtryFree((void*)address, __PRETTY_FUNCTION__, __FILE__, __LINE__)

/*
#define _malloc1(size) _Xmalloc(size, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _free(address) _Xfree(address, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#define malloc2(size, text) _Xmalloc(size, text, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define _malloc2(size, text) _Xmalloc(size, text, __PRETTY_FUNCTION__, __FILE__, __LINE__)

*/






#define MallocCacheCount 8
#define MallocCacheSize 16

extern void* MallocCache16BytesAddr [MallocCacheCount];
extern bool MallocCache16BytesFree [MallocCacheCount];
extern uint64_t MallocCache16BytesTime [MallocCacheCount];





bool ExpandHeap(size_t length);

extern void* heapStart;

extern int64_t heapCount;

extern int64_t usedHeapCount;
extern int64_t mallocCount;
extern int64_t freeCount;
extern int64_t activeMemFlagVal;
extern int64_t lastFreeSize;

//inline void* operator new(size_t size) {return _Malloc(size);}
//inline void* operator new[](size_t size) {return _Malloc(size);}
//inline void operator delete(void* p) {_Free(p);}

#include "new.hpp"



