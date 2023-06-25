#include "MStackM.h"
//#include "../osdata.h"
#include "../../kernelStuff/other_IO/serial/serial.h"

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
    Serial::Writeln("STACK TRACE: ({} Elements)\n", to_string(count), true);
    if (size != 1000)
    {
        if (size > 50)
            size = 50;
        for (int i = 0; i < size; i++)
        {
            int index = (size - i) - 1;
            if (stack[index].line != -1)
            {
                renderer->Println("> At \"{}\"", stack[index].name, col);
                renderer->Print("  > in file \"{}\" ", stack[index].filename, col);
                renderer->Println("at line \"{}\"", to_string(stack[index].line), col);

                Serial::Writeln("> At \"{}\"", stack[index].name, true);
                Serial::Write("  > in file \"{}\" ", stack[index].filename, true);
                Serial::Writeln("at line \"{}\"", to_string(stack[index].line), true);
            }
        }
    }
    else
    {
        if (size > 50)
            size = 50;
        for (int i = 0; i < size; i++)
        {
            int index = i;
            if (stack[index].line != -1)
            {
                renderer->Println("> At \"{}\"", stack[index].name, col);
                renderer->Print("  > in file \"{}\" ", stack[index].filename, col);
                renderer->Println("at line \"{}\"", to_string(stack[index].line), col);
                
                Serial::Writeln("> At \"{}\"", stack[index].name, true);
                Serial::Write("  > in file \"{}\" ", stack[index].filename, true);
                Serial::Writeln("at line \"{}\"", to_string(stack[index].line), true);
            }
        }
    }
    renderer->Println();
    Serial::Writeln();

#else

    renderer->Println("M-Stack traces are disabled!");
    renderer->Println();
    Serial::Writeln("M-Stack traces are disabled!");
    Serial::Writeln();

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

    MStack BenchmarkStackArr1[BenchmarkStackSize];
    MStack BenchmarkStackArr2[BenchmarkStackSize];
    MStack BenchmarkStackArrSave[BenchmarkStackSize];

    int64_t BenchmarkStackPointer1 = 0;
    int64_t BenchmarkStackPointer2 = 0;
    int64_t BenchmarkStackPointerSave = 0;
    bool BenchmarkEnabled = false;
    int BenchmarkMode = 0;
};



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










void _AddTheBenchmark(MStack thing)
{
    if (osData.tempCrash)
        return;
    if (PIT::Inited)
        thing.time = PIT::TimeSinceBootMicroS();
    else
        thing.time = 0;
    thing.layer = MStackData::stackPointer + 1;   
    thing.close = false;

    if (MStackData::BenchmarkMode == 0)
    {
        if (MStackData::BenchmarkStackPointer1 >= BenchmarkStackSize)
        {
            return;
            MStackData::BenchmarkStackPointer1 = 0;
            MStackData::BenchmarkEnabled = false;
            Panic("BENCHMARK OUT OF BOUNDS!", true);
        }
        MStackData::BenchmarkStackArr1[MStackData::BenchmarkStackPointer1++] = thing;
    }
    else
    {
        if (MStackData::BenchmarkStackPointer2 >= BenchmarkStackSize)
        {
            return;
            MStackData::BenchmarkStackPointer2 = 0;
            MStackData::BenchmarkEnabled = false;
            Panic("BENCHMARK OUT OF BOUNDS!", true);
        }
        MStackData::BenchmarkStackArr2[MStackData::BenchmarkStackPointer2++] = thing;
    }
}

void _RemoveBenchmark(MStack thing)
{
    if (osData.tempCrash)
        return;
    if (PIT::Inited)
        thing.time = PIT::TimeSinceBootMicroS();
    else
        thing.time = 0;
    thing.layer = MStackData::stackPointer;   
    thing.close = true;

    if (MStackData::BenchmarkMode == 0)
    {
        if (MStackData::BenchmarkStackPointer1 >= BenchmarkStackSize)
        {
            return;
            MStackData::BenchmarkStackPointer1 = 0;
            MStackData::BenchmarkEnabled = false;
            Panic("BENCHMARK OUT OF BOUNDS!", true);
        }
        MStackData::BenchmarkStackArr1[MStackData::BenchmarkStackPointer1++] = thing;
    }
    else
    {
        if (MStackData::BenchmarkStackPointer2 >= BenchmarkStackSize)
        {
            return;
            MStackData::BenchmarkStackPointer2 = 0;
            MStackData::BenchmarkEnabled = false;
            Panic("BENCHMARK OUT OF BOUNDS!", true);
        }
        MStackData::BenchmarkStackArr2[MStackData::BenchmarkStackPointer2++] = thing;
    }
}














void AddToTheMStack(MStack thing)
{
    if (MStackData::BenchmarkEnabled)
        _AddTheBenchmark(thing);

    if (MStackData::stackPointer < 1000)
    {
        MStackData::stackArr[MStackData::stackPointer] = thing;
        MStackData::stackPointer++;
    }
    else
        Panic("Stack overflow", true);
}

void RemoveTheLastElementFromTheMStack()
{
    if (MStackData::BenchmarkEnabled)
        _RemoveBenchmark(MStackData::stackArr[MStackData::stackPointer-1]);

    if (MStackData::stackPointer > 0)
        MStackData::stackPointer--;
    else
    {
        MStackData::stackPointer = 900;
        Panic("Stack underflow", true);
    }
}


void SaveBenchmarkStack(int mode)
{
    if (mode == 0)
    {
        for (int i = 0; i < MStackData::BenchmarkStackPointer1; i++)
            MStackData::BenchmarkStackArrSave[i] = MStackData::BenchmarkStackArr1[i];
        MStackData::BenchmarkStackPointerSave = MStackData::BenchmarkStackPointer1;
    }
    else
    {
        for (int i = 0; i < MStackData::BenchmarkStackPointer2; i++)
            MStackData::BenchmarkStackArrSave[i] = MStackData::BenchmarkStackArr2[i];
        MStackData::BenchmarkStackPointerSave = MStackData::BenchmarkStackPointer2;
    }
}