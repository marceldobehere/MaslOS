//#include "heap.h"
#include <stddef.h>
#include "new2.h"
const char* _X__file__ = "unknown";
const char* _X__func__ = "unknown";
size_t _X__line__ = 0;

void* operator new(size_t size) {
    void *ptr = _Ymalloc(size, _X__func__, _X__file__, _X__line__);
    _X__file__ = "unknown";
    _X__func__ = "unknown";
    _X__line__ = 0;
    return ptr;
}