static int __TEST_VAR__ = 5;






//void Log(const char* message);

class OS_Window
{
    public:
    void* window;
    OS_Window(void* window);
    void Log(const char* msg);
};


struct KernelAppData
{
    bool test;
    OS_Window* window;
};


