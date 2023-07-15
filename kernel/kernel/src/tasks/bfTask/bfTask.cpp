#include "bfTask.h"
#include "../../kernelStuff/other_IO/pit/pit.h"
#include "../../memory/heap.h"
#include "../../interrupts/interrupts.h"
#include "../../cStdLib/cstrTools.h"
#include "../../OSDATA/MStack/MStackM.h"


TaskBF::TaskBF(const char* code, Window* window)
{
    if (StrEquals(code, "hello-world"))
        this->code = StrCopy(">++++++++[<+++++++++>-]<.>++++[<+++++++>-]<+.+++++++..+++.>>++++++[<+++++++>-]<++.------------.>++++++[<+++++++++>-]<+.<.+++.------.--------.>>>++++[<++++++++>-]<+.");
    else if (StrEquals(code, "number-compare"))
        this->code = StrCopy(">->>[-]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.+++++.++.+++++.-.----------------------------------------------------.++++++++++++++++++++++++++++++++++++++++++++++.+++++++.--------.-----------.+++.+++++++++++++.--------------------------------------------------.+++++++++++++++++++++++++++++++++++++++++++++++.-.---------.-----------.--------------------------.<,>>>>>>>>>>>>>>>>>>>[-]<<<<<<<<<<<<[-]<<<<<<<[>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<+<<<<<<<-]>>>>>>>[<<<<<<<+>>>>>>>-]<<<<<<[-]>>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<<<.[-]++++++++++.[-]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.+++++.++.+++++.-.----------------------------------------------------.++++++++++++++++++++++++++++++++++++++++++++++.+++++++.--------.-----------.+++.+++++++++++++.--------------------------------------------------.++++++++++++++++++++++++++++++++++++++++++++++++++++.+++.--------.---------------------.--------------------------.<,>>>>>>>>>>>>>>>>>>>>[-]<<<<<<<<<<<<<[-]<<<<<<<[>>>>>>>>>>>>>>>>>>>>+<<<<<<<<<<<<<+<<<<<<<-]>>>>>>>[<<<<<<<+>>>>>>>-]<<<<<<[-]>>>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<<<<.>[-]>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<[-]>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<[-]++<<[<->-]<[>>>[-]<<<[-]]>>>[<<<<[-]++++++++++.[-]>>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<<<.[-]+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.[-]>>>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<<<<.>[-]>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<[-]>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<[-]++<<[<->-]<[>>>[-]<<<[-]]>>>]<<[-]>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<+>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<<[-]>>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<+>>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<[-]>[-]<<-[<->>[-]>>>[-]<<<<<[>>+>>>+<<<<<-]>>>>>[<<<<<+>>>>>-]<<[-]++<[>[-]<[-]][-]>>>[-]<<[<+>>>+<<-]>>[<<+>>-]<<<[>++<<[-]+>[-]]<-]>>[<<<<[-]++++++++++.[-]>>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<<<.[-]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.[-]>>>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<<<<.>>[-]>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<+>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<<[-]>>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<+>>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<[-]>[-]<<-[<->>[-]>>>[-]<<<<<[>>+>>>+<<<<<-]>>>>>[<<<<<+>>>>>-]<<[-]++<[>[-]<[-]][-]>>>[-]<<[<+>>>+<<-]>>[<<+>>-]<<<[>++<<[-]+>[-]]<-]>>]<<[-]>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<<<[-]>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<[-]>[-]<<-[<->>[-]>>>[-]<<<<<[>>+>>>+<<<<<-]>>>>>[<<<<<+>>>>>-]<<[-]++<[>[-]<[-]][-]>>>[-]<<[<+>>>+<<-]>>[<<+>>-]<<<[>++<<[-]+>[-]]<-]>>[<<<<[-]++++++++++.[-]>>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<<<<.[-]++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.[-]>>>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<<<+>>>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<<<<.>>[-]>>>>[-]>>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>+>>>>>>>>>>>>>-]<<<<<<<<<<<<<[>>>>>>>>>>>>>+<<<<<<<<<<<<<-]<<<<<[-]>>>>>[-]>>>>>>>>>>>>[<<<<<<<<<<<<<<<<<+>>>>>+>>>>>>>>>>>>-]<<<<<<<<<<<<[>>>>>>>>>>>>+<<<<<<<<<<<<-]<<<[-]>[-]<<-[<->>[-]>>>[-]<<<<<[>>+>>>+<<<<<-]>>>>>[<<<<<+>>>>>-]<<[-]++<[>[-]<[-]][-]>>>[-]<<[<+>>>+<<-]>>[<<+>>-]<<<[>++<<[-]+>[-]]<-]>>]>>>>>>>>>>>>>>>>");
    else
        this->code = StrCopy(code);
    
    this->window = window;
    pIndex = 0;
    mIndex = 0;
    memLen = 100000;
    mem = (uint8_t*)_Malloc(memLen, "BF MEM");
    for (int i = 0; i < memLen; i++)
        mem[i] = 0;
    waitInput = false;
    len = StrLen(this->code);
    done = false;
    type = TaskType::BF;


    ((TerminalInstance*)window->instance)->userlen = 0;
    ((TerminalInstance*)window->instance)->takeInput = false;
    waitInput = false;
}

void TaskBF::Do()
{
    AddToStack()
    if (pIndex >= len || done)
    {
        done = true;
        RemoveFromStack()
        return;
    }

    NewTerminalInstance* newTerm = (NewTerminalInstance*)(((TerminalInstance*)(window->instance))->newTermInstance);


    if (waitInput)
    {
        // Get Input
        ((TerminalInstance*)window->instance)->takeInput = true;
        if (((TerminalInstance*)window->instance)->userlen != 0)
        {
            char input = ((TerminalInstance*)window->instance)->terminalInput[0];
            mem[mIndex] = input;
            ((TerminalInstance*)window->instance)->userlen = 0;
            ((TerminalInstance*)window->instance)->takeInput = false;
            waitInput = false;
            pIndex++;
        }
        else
        {
            RemoveFromStack()
            return;
        }
    }

    for (int t = 0; t < cyclesPerCall; t++)
    {
        if (pIndex >= len || done)
        {
            done = true;
            RemoveFromStack()
            return;
        }

        if (code[pIndex] == '<')
        {
            mIndex--;
            if (mIndex < 0)
                mIndex += memLen;
            //window->renderer->Println("MEMINDEX: {}", to_string(mIndex), Colors.yellow);
            //while (true);
        }
        else if (code[pIndex] == '>')
        {
            mIndex++;
            if (mIndex >= memLen)
                mIndex -= memLen;
        }
        else if (code[pIndex] == '+')
        {
            mem[mIndex]++;
        }
        else if (code[pIndex] == '-')
        {
            mem[mIndex]--;
        }
        else if (code[pIndex] == '.')
        {
            if (mem[mIndex] == '\n')
                newTerm->Println();
            else
                newTerm->Print((char)mem[mIndex]);
        }
        else if (code[pIndex] == ',')
        {
            waitInput = true;

            RemoveFromStack()
            return;
        }
        else if (code[pIndex] == '[')
        {
            if (mem[mIndex] == 0)
            {
                int l = 1;
                while (l > 0 || pIndex >= len)
                {
                    pIndex++;
                    if (code[pIndex] == '[')
                        l++;
                    else if (code[pIndex] == ']')
                        l--;
                }
            }
        }
        else if (code[pIndex] == ']')
        {
            if (mem[mIndex] != 0)
            {
                int l = 1;
                while (l > 0 || pIndex < 0)
                {
                    pIndex--;
                    if (code[pIndex] == ']')
                        l++;
                    else if (code[pIndex] == '[')
                        l--;
                }
            }
        }


        pIndex++;
    }
    

    RemoveFromStack()
}

void TaskBF::Free()
{
    _Free((void*)code);
    _Free((void*)mem);
}

TaskBF* NewBFTask(const char* code, Window* window)
{
    TaskBF* task = (TaskBF*)_Malloc(sizeof(TaskBF));
    *task = TaskBF(code, window);
    return task;
}