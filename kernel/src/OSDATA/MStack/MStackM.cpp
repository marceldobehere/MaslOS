#include "MStackM.h"
//#include "../osdata.h"

void PrintMStackTrace(MStack stack[], int64_t size, BasicRenderer* renderer, uint32_t col)
{
#if RECORD_STACK_TRACE  
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        int index = (size - i) - 1;
        if (stack[index].line != -1)
            count++;
    }
    renderer->Println("STACK TRACE: ({} Elements)\n", to_string(count), col);
    if (size > 50)
        size = 50;
    for (int i = 0; i < size; i++)
    {
        int index = (size - i) - 1;
        if (stack[index].line != -1)
        {
            renderer->Println("> At \"{}\"", stack[index].name, col);
            renderer->Println("  > in file \"{}\"", stack[index].filename, col);
            renderer->Println("  > At line: {}", to_string(stack[index].line), col);
            renderer->Println();
        }
    }
    renderer->Println();

#else

    renderer->Println("M-Stack traces are disabled!");
    renderer->Println();

#endif




    // stackframe* stk;
    // asm("mov %%rbp, %0" : "=r"(stk) ::);

    // renderer->Println("STACK TRACE:\n", col);
    // for (int i = 0; i < 10 && stk != NULL; i++)
    // {
    //     renderer->Println("  > At \"{}\"", ConvertHexToString(stk->eip), col);
    //     //renderer->Println(" in file \"{}\"", stack[i].filename, col);
    //     stk = stk->ebp;
    // }
    // renderer->Println();
}

void PrintMStackTrace(MStack stack[], int64_t size)
{
    PrintMStackTrace(stack, size, GlobalRenderer, Colors.white);
}

namespace MStackData
{
    MStack stackArr[1000];
    int64_t stackPointer = 0;
}


// void AddToTheMStack(MStack thing)
// {
//     if (osData.stackPointer < 1000)
//     {
//         osData.stackArr[osData.stackPointer] = thing;
//         osData.stackPointer++;
//     }
//     else
//         Panic("Stack overflow");
// }

// void RemoveTheLastElementFromTheMStack()
// {
//     if (osData.stackPointer > 0)
//         osData.stackPointer--;
//     else
//     {
//         osData.stackPointer = 900;
//         Panic("Stack underflow");
//     }
// }


