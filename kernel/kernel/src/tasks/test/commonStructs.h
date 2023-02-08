#include "stdint.h"


static int __TEST_VAR__ = 5;



//void Log(const char* message);

struct OS_Framebuffer
{
    uint64_t height;
    uint64_t width;
    uint64_t pps;
    uint64_t size;
    void* start;
};

class OS_Window
{
    public:
    void* window;
    OS_Framebuffer windowFramebuffer;
    OS_Window(void* window);
    void Log(const char* msg);
};


struct KernelAppData
{
    bool test;
    OS_Window* window;
    void* (*OS_Malloc)(uint64_t);
    void (*OS_Free)(void*);
};


