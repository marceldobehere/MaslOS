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
    *list = List<ConsoleChar>(2);
    textData.add(list);
    RemoveFromStack();
    return list;
}

void NewTerminalInstance::WriteStringIntoList(const char* chrs, const char* var)
{
    WriteStringIntoList(chrs, var, true);
}

void NewTerminalInstance::WriteStringIntoList(const char* chrs, const char* var, bool allowEscape)
{   
    AddToStack();

    
    //for (int x = 0; x < 5; x++)
    //    GlobalRenderer->ClearDotted(Colors.bblue);

    allowEscape = false;
    int len = StrLen(chrs);
    
    List<ConsoleChar>* currList = textData.elementAt(textData.getCount() - 1);

    uint32_t fg = foregroundColor;
    uint32_t bg = backgroundColor;

    for (int index = 0; index < len; index++)
    {
        //GlobalRenderer->Println("AAAAA");
        if (chrs[index] == '\n')
        {
            currList = AddNewLine();
            currList = textData.elementAt(textData.getCount() - 1);
        }
        else if (chrs[index] == '\r')
            ;// Maybe implement moving char
        else if (chrs[index] == '{' && allowEscape && var != NULL)
        {
            if (chrs[index + 1] == '}')
            {
                WriteStringIntoList(var, NULL, false);
                currList = textData.elementAt(textData.getCount() - 1);
                index++;
            }
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
                    currList->add(ConsoleChar('?', fg, bg));
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
                    currList->add(ConsoleChar('?', fg, bg));
                else
                {
                    index++;
                    bg = ConvertStringToHex(&chrs[index]);
                    index += 5;
                }
            }
            else
                currList->add(ConsoleChar(chrs[index], fg, bg));
        }
        else
            currList->add(ConsoleChar(chrs[index], fg, bg));
        
        //currList->add(ConsoleChar(str[i], fg, bg));
    }
    
    //for (int x = 0; x < 5; x++)
    //    GlobalRenderer->ClearDotted(Colors.bgreen);

    RemoveFromStack();
}

void NewTerminalInstance::WriteVarStringIntoList(const char* chrs, dispVar vars[])
{
    //return;
    AddToStack();
    int len = StrLen(chrs);
    
    List<ConsoleChar>* currList = textData.elementAt(textData.getCount() - 1);

    uint32_t fg = foregroundColor;
    uint32_t bg = backgroundColor;

    for (int index = 0; index < len; index++)
    {
        if (chrs[index] == '\n')
            currList = AddNewLine();
        else if (chrs[index] == '\r')
            ;// Maybe implement moving char
        else if (chrs[index] == '{' && vars != NULL)
        {
            uint64_t start = index;
            while (chrs[index] != 0 && chrs[index] != '}')
                index++;
            if (chrs[index] == '}')
            {
                char* sub = StrSubstr(chrs, start+1, index-(start+1));
                //GlobalRenderer->Print("[{}]", to_string(index-(start+1)), Colors.yellow);
                //GlobalRenderer->Print("<");
                //GlobalRenderer->Print(sub);
                //GlobalRenderer->Print(">");
                uint64_t indx = to_int(sub);
                //Print("[{}]", to_string(indx), Colors.yellow);
                free(sub);
                //GlobalRenderer->Print("[{}]", to_string(indx), Colors.yellow);
                
                WriteStringIntoList(PrintArg(vars[indx]), NULL, false);
                index++;
            }
            index--;
        }
        else if (chrs[index] == '\\')
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
                    currList->add(ConsoleChar('?', fg, bg));
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
                    currList->add(ConsoleChar('?', fg, bg));
                else
                {
                    index++;
                    bg = ConvertStringToHex(&chrs[index]);
                    index += 5;
                }
            }
            else
                currList->add(ConsoleChar(chrs[index], fg, bg));
        }
        else
            currList->add(ConsoleChar(chrs[index], fg, bg));
        
        //currList->add(ConsoleChar(str[i], fg, bg));
    }
    
    RemoveFromStack();
}










void ClearCharArr(ConsoleChar* charArr, int sizeX, int sizeY)
{
    ConsoleChar tempChar = ConsoleChar(' ', Colors.white, Colors.black);
    for (int i = 0; i < sizeX * sizeY; i++)
        charArr[i] = tempChar;
}

void ReInitCharArrWithSize(ConsoleChar** charArr, int sizeX, int sizeY)
{
    *charArr = (ConsoleChar*)malloc((sizeX * sizeY * sizeof(ConsoleChar)));
    ClearCharArr(*charArr, sizeX, sizeY);
}


NewTerminalInstance::NewTerminalInstance(/*OSUser* user, */Window* window)
{
    AddToStack();
    textData = List<List<ConsoleChar>*>(4);
    AddNewLine();
    instanceType = InstanceType::NewTerminal;
    //this->currentUser = user;
    this->window = window;
    backgroundColor = Colors.black;
    foregroundColor = Colors.white;
    scrollX = 0;
    scrollY = 0;
    oldScrollX = 0;
    oldScrollY = 0;
    oldWidth = window->size.width;
    oldHeight = window->size.height;
    //tempPixels = (ConsoleChar*)malloc(((window->size.width/8)*(window->size.height/16) * sizeof(ConsoleChar)));
    ReInitCharArrWithSize(&tempPixels, (window->size.width/8), (window->size.height/16));
    ReInitCharArrWithSize(&tempPixels2, (window->size.width/8), (window->size.height/16));
    RemoveFromStack();
}
    
void NewTerminalInstance::WriteText(const char* text)
{
    AddToStack();
    //window->brenderer->Println(text);
    //window->BlitBackbuffer();
    WriteStringIntoList(text, NULL);
    AddNewLine();
    Render();
    RemoveFromStack();
}

void NewTerminalInstance::RenderCharChanges()
{
    AddToStack();
    int sizeX = oldWidth / 8;
    int sizeY = oldHeight / 16;
    ClearCharArr(tempPixels2, sizeX, sizeY);

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

        int sDy = max(dy1, 0);
        int sDx = max(dx1, 0);

        int maxY = min(dy2 - 1, textData.getCount() - 1);
        for (int dy = sDy; dy <= maxY; dy++)
        {
            List<ConsoleChar>* tList = textData.elementAt(dy);

            int maxX = min(dx2 - 1, tList->getCount() - 1);
            for (int dx = sDx; dx <= maxX; dx++)
            {
                ConsoleChar chr = tList->elementAt(dx);
                if (((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) < 0) || ((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) > (oldWidth / 8)*(oldHeight / 16)))
                        Panic("OUT OF BOUNDS OMGGGGGGGGGGGGGGGGGGG!");
                tempPixels2[(dx - dx1) + ((dy - dy1) * (sizeX))] = chr;
            }
        }
        RemoveFromStack(); 
    }

    for (int y = 0; y < sizeY; y++)
        for (int x = 0; x < sizeX; x++)
            if (tempPixels[x + (y * sizeX)] != tempPixels2[x + (y * sizeX)])
            {
                ConsoleChar chr = tempPixels2[x + (y * sizeX)];
                // if (chr.chr == ' ')
                //    chr.chr = '?';
                // chr.fg = Colors.yellow;
                tempPixels[x + (y * sizeX)] = chr;
                window->renderer->putChar(chr.chr, x*8 - (scrollX % 8), y*16 - (scrollY % 16), chr.fg, chr.bg);
            }
    //window->BlitBackbuffer();
    RemoveFromStack();
}

void NewTerminalInstance::Render()
{
    AddToStack();
    // RemoveFromStack();
    // return;
    if (scrollX == oldScrollX && scrollY == oldScrollY && oldHeight == window->size.height && oldWidth == window->size.width)
    {
        //osData.drawBackground = !osData.drawBackground;
        RenderCharChanges();
    }
    else
    {
        oldScrollX = scrollX;
        oldScrollY = scrollY;
        if (oldHeight != window->size.height || oldWidth != window->size.width)
        {
            oldWidth = window->size.width;
            oldHeight = window->size.height;
            free(tempPixels);
            ReInitCharArrWithSize(&tempPixels, (window->size.width/8), (window->size.height/16));
            free(tempPixels2);
            ReInitCharArrWithSize(&tempPixels2, (window->size.width/8), (window->size.height/16));
        }

        window->renderer->Clear(backgroundColor);
        ClearCharArr(tempPixels, (window->size.width/8), (window->size.height/16));
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

            int sDy = max(dy1, 0);
            int sDx = max(dx1, 0);

            int maxY = min(dy2 - 1, textData.getCount() - 1);
            for (int dy = sDy; dy <= maxY; dy++)
            {
                List<ConsoleChar>* tList = textData.elementAt(dy);

                int maxX = min(dx2 - 1, tList->getCount() - 1);
                for (int dx = sDx; dx <= maxX; dx++)
                {
                    ConsoleChar chr = tList->elementAt(dx);
                    window->renderer->putChar(chr.chr, dx*8-fx1, dy*16-fy1, chr.fg, chr.bg);

                    if (((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) < 0) || ((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) > (oldWidth / 8)*(oldHeight / 16)))
                        Panic("OUT OF BOUNDS OMGGGGGGGGGGGGGGGGGGG!");
                    tempPixels[(dx - dx1) + ((dy - dy1) * (oldWidth / 8))] = chr;
                }
            }
            RemoveFromStack(); 
        }

        //window->BlitBackbuffer();
        //RemoveFromStack();
    }
    //RenderCharChanges();
    RemoveFromStack();
    return;
}

void NewTerminalInstance::Clear()
{
    AddToStack();
    window->brenderer->Clear(backgroundColor);
    window->renderer->Clear(backgroundColor);
    ClearListList(&textData);
    AddNewLine();
    Render();
    window->BlitBackbuffer();
    RemoveFromStack();
}

void NewTerminalInstance::Free()
{
    ClearListList(&textData);
    free(tempPixels);
    free(tempPixels2);
}


void NewTerminalInstance::Println()
{
    AddNewLine();
    Render();
}
void NewTerminalInstance::Print(const char* msg)
{
    WriteStringIntoList(msg, NULL, true);
    Render();
}
void NewTerminalInstance::Print(const char* chrs, const char* var)
{
    WriteStringIntoList(chrs, var, true);
    Render();
}

char tempCharData[2]; 
void NewTerminalInstance::Print(char chr)
{
    tempCharData[0] = chr;
    tempCharData[1] = 0;
    WriteStringIntoList(tempCharData, NULL, true);
    Render();
}
void NewTerminalInstance::Print(const char* chrs, dispVar vars[])
{
    WriteVarStringIntoList(chrs, vars);
    Render();
}
void NewTerminalInstance::DeleteLastCharInLine()
{
    List<ConsoleChar>* currList = textData.elementAt(textData.getCount() - 1);
    if (currList->getCount() > 0)
        currList->removeLast();

    Render();
}

void NewTerminalInstance::Print(const char* chrs, uint32_t col)
{
    uint32_t tCol = foregroundColor;
    foregroundColor = col;
    Print(chrs);
    foregroundColor = tCol;
}
void NewTerminalInstance::Print(const char* chrs, const char* var, uint32_t col)
{
    uint32_t tCol = foregroundColor;
    foregroundColor = col;
    Print(chrs, var);
    foregroundColor = tCol;
}

void NewTerminalInstance::Print(const char *chrs, dispVar vars[], uint32_t col)
{
    uint32_t tCol = foregroundColor;
    foregroundColor = col;
    Print(chrs, vars);
    foregroundColor = tCol;
}





void NewTerminalInstance::Println(const char* msg)
{
    Print(msg);
    Println();
}
void NewTerminalInstance::Println(const char* chrs, const char* var)
{
    Print(chrs, var);
    Println();
}
void NewTerminalInstance::Println(const char* chrs, uint32_t col)
{
    Print(chrs, col);
    Println();
}
void NewTerminalInstance::Println(const char* chrs, const char* var, uint32_t col)
{
    Print(chrs, var, col);
    Println();
}

void NewTerminalInstance::Println(const char* chrs, dispVar vars[])
{
    Print(chrs, vars);
    Println();
}

void NewTerminalInstance::Println(const char *chrs, dispVar vars[], uint32_t col)
{
    Print(chrs, vars, col);
    Println();
}