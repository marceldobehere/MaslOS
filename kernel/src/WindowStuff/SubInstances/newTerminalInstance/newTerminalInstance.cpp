#include "newTerminalInstance.h"
#include "../../../memory/heap.h"
#include "../../../OSDATA/MStack/MStackM.h"

void ClearListList(List<List<ConsoleChar>*>* list)
{
    AddToStack();
    while(list->getCount() > 0)
    {
        List<ConsoleChar>* tList = list->elementAt(0);
        tList->free();
        free(tList);
        list->removeFirst();
    }
    RemoveFromStack();
}

List<ConsoleChar>* NewTerminalInstance::AddNewLine()
{
    AddToStack();
    List<ConsoleChar>* list = (List<ConsoleChar>*)malloc(sizeof(List<ConsoleChar>), "New List of console chars");
    *list = List<ConsoleChar>(1);
    textData.add(list);
    RemoveFromStack();
    return list;
}

void NewTerminalInstance::WriteStringIntoList(const char* chrs)
{
    WriteStringIntoList(chrs, true);
}

void NewTerminalInstance::WriteStringIntoList(const char* chrs, bool allowEscape)
{   
    AddToStack();
    int len = StrLen(chrs);
    

    List<ConsoleChar>* currList = textData.elementAt(textData.getCount() - 1);

    uint32_t fg = foregroundColor;
    uint32_t bg = backgroundColor;

    for (int index = 0; index < len; index++)
    {

        if (chrs[index] == '\n')
        {
            currList = AddNewLine();
        }
        else if (chrs[index] == '\r')
        {
            // Maybe implement moving char
        }
        else if (chrs[index] == '\\' && allowEscape)
        {
            if (chrs[index + 1] == '\\')
            {
                index++;
                currList->add(ConsoleChar('\\', fg, bg));
            }
            else if (chrs[index + 1] == '%')
            {
                index++;
                currList->add(ConsoleChar('%', fg, bg));
            }
            else if (chrs[index + 1] == '{')
            {
                index++;
                currList->add(ConsoleChar('{', fg, bg));
            }
            else if (chrs[index + 1] == '}')
            {
                index++;
                currList->add(ConsoleChar('}', fg, bg));
            }
            else if (chrs[index + 1] == 'F') // foreground color
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    currList->add(ConsoleChar('?', fg, bg));
                }
                else
                {
                    index++;
                    fg = ConvertStringToHex(&chrs[index]);
                    index += 5;
                }
            }
            else if (chrs[index + 1] == 'B') // foreground color
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    currList->add(ConsoleChar('?', fg, bg));
                }
                else
                {
                    index++;
                    bg = ConvertStringToHex(&chrs[index]);
                    index += 5;
                }
            }
            else
            {
                currList->add(ConsoleChar(chrs[index], fg, bg));
            }
        }
        else
        {
            currList->add(ConsoleChar(chrs[index], fg, bg));
        }
        //currList->add(ConsoleChar(str[i], fg, bg));
    }
    
    RemoveFromStack();
}


NewTerminalInstance::NewTerminalInstance(OSUser* user, Window* window)
{
    AddToStack();
    textData = List<List<ConsoleChar>*>(4);
    AddNewLine();
    instanceType = InstanceType::NewTerminal;
    this->currentUser = user;
    this->window = window;
    backgroundColor = Colors.black;
    foregroundColor = Colors.white;
    scrollX = 0;
    scrollY = 0;
    RemoveFromStack();
}
    
void NewTerminalInstance::WriteText(const char* text)
{
    AddToStack();
    //window->brenderer->Println(text);
    //window->BlitBackbuffer();
    WriteStringIntoList(text);
    AddNewLine();
    Render();
    RemoveFromStack();
}

void NewTerminalInstance::Render()
{
    AddToStack();
    window->brenderer->Clear(backgroundColor);

    if (textData.getCount() == 0)
    {
        window->BlitBackbuffer();
        RemoveFromStack(); 
        return;
    }

    {
        AddToStack();
        int fy1 = scrollY;
        int fy2 = fy1 + window->size.height;
        int dy1 = fy1 / 16;
        int dy2 = fy2 / 16;

        int fx1 = scrollX;
        int fx2 = fx1 + window->size.width;
        int dx1 = fx1 / 8;
        int dx2 = fx2 / 8;

        int maxY = min(dy2, textData.getCount() - 1);
        for (int dy = max(dy1, 0); dy <= maxY; dy++)
        {
            List<ConsoleChar>* tList = textData.elementAt(dy);

            int maxX = min(dx2, tList->getCount() - 1);
            for (int dx = max(dx1, 0); dx <= maxX; dx++)
            {
                ConsoleChar chr = tList->elementAt(dx);
                window->brenderer->putChar(chr.chr, dx*8-fx1, dy*16-fy1, chr.fg, chr.bg);
            }
        }
        RemoveFromStack(); 
    }


    window->BlitBackbuffer();
    RemoveFromStack();
}

void NewTerminalInstance::Clear()
{
    AddToStack();
    ClearListList(&textData);
    AddNewLine();
    Render();
    window->BlitBackbuffer();
    RemoveFromStack();
}

void NewTerminalInstance::Free()
{
    ClearListList(&textData);
    
}