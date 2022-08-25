#pragma once

#include <stdint.h>
#include <stddef.h>


struct HeapSegHdr
{
    size_t length;
    HeapSegHdr* next;
    HeapSegHdr* last;
    const char* text = "<DEF>";
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr* Split(size_t splitLength);
    uint32_t magicNum;
};


void InitializeHeap(void* heapAddress, size_t pageCount);

void* malloc(size_t size);
void free(void* address);

void* _malloc(size_t size);
void _free(void* address);

void* malloc(size_t size, const char* text);
void* _malloc(size_t size, const char* text);

void ExpandHeap(size_t length);

extern void* heapStart;

extern int64_t heapCount;

extern int64_t mallocCount;
extern int64_t freeCount;

inline void* operator new(size_t size) {return malloc(size);}
inline void* operator new[](size_t size) {return malloc(size);}
inline void operator delete(void* p) {free(p);}