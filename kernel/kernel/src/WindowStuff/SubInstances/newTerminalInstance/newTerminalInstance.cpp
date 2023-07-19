#include "newTerminalInstance.h"
#include "../../../memory/heap.h"
#include "../../../osData/MStack/MStackM.h"
#include "../../..//devices/serial/serial.h"

void ClearListList(List<List<ConsoleChar>*>* list)
{
    AddToStack();
    while(list->getCount() > 0)
    {
        List<ConsoleChar>* tList = list->elementAt(0);
        tList->free();
        _Free(tList);
        list->removeFirst();
    }
    RemoveFromStack();
}

List<ConsoleChar>* NewTerminalInstance::AddNewLine()
{
    AddToStack();
    if (redirectToSerial)
        Serial::Writeln();

    List<ConsoleChar>* list = (List<ConsoleChar>*)_Malloc(sizeof(List<ConsoleChar>), "New List of console chars");
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

    if ((uint64_t)chrs < 100)
        Panic("STR IS NULL", true);

    //allowEscape = false;
    int len = StrLen(chrs);
    
    if (var != NULL)
        var = StrCopy(var);


    List<ConsoleChar>* currList = textData.elementAt(textData.getCount() - 1);
    if (currList == 0)
        Panic("LAST LIST IS NULL", true);


    uint32_t fg = foregroundColor;
    uint32_t bg = backgroundColor;

    for (int index = 0; index < len; index++)
    {
        if (currList == NULL)
            Panic("LAST LIST IS NULL 2", true);

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
                if (redirectToSerial)
                    Serial::Write('\\');
            }
            else if (chrs[index + 1] == '%')
            {
                index++;
                currList->add(ConsoleChar('%', fg, bg));
                if (redirectToSerial)
                    Serial::Write('%');
            }
            else if (chrs[index + 1] == '{')
            {
                index++;
                currList->add(ConsoleChar('{', fg, bg));
                if (redirectToSerial)
                    Serial::Write('{');
            }
            else if (chrs[index + 1] == '}')
            {
                index++;
                currList->add(ConsoleChar('}', fg, bg));
                if (redirectToSerial)
                    Serial::Write('}');
            }
            else if (chrs[index + 1] == 'F') // foreground color
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    currList->add(ConsoleChar('?', fg, bg));
                    if (redirectToSerial)
                        Serial::Write('?');
                }
                else
                {
                    index++;
                    fg = ConvertStringToHex(&chrs[index]);
                    // ignore switching col in serial 
                    index += 5;
                }
            }
            else if (chrs[index + 1] == 'B') // foreground color
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    currList->add(ConsoleChar('?', fg, bg));
                    if (redirectToSerial)
                        Serial::Write('?');
                }
                else
                {
                    index++;
                    bg = ConvertStringToHex(&chrs[index]);
                    // ignore switching col in serial 
                    index += 5;
                }
            }
            else
            {
                currList->add(ConsoleChar(chrs[index], fg, bg));
                if (redirectToSerial)
                    Serial::Write(chrs[index]);
            }
        }
        else
        {
            currList->add(ConsoleChar(chrs[index], fg, bg));
            if (redirectToSerial)
                Serial::Write(chrs[index]);
        }
        
        //currList->add(ConsoleChar(str[i], fg, bg));
    }


    if (var != NULL)
        _Free((void*)var);
    RemoveFromStack();
}


void NewTerminalInstance::WriteVarStringIntoList(const char* chrs, dispVar vars[])
{
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
                _Free(sub);
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
                if (redirectToSerial)
                    Serial::Write('\\');
            }
            else if (chrs[index + 1] == '%')
            {
                index++;
                currList->add(ConsoleChar('%', fg, bg));
                if (redirectToSerial)
                    Serial::Write('%');
            }
            else if (chrs[index + 1] == '{')
            {
                index++;
                currList->add(ConsoleChar('{', fg, bg));
                if (redirectToSerial)
                    Serial::Write('{');
            }
            else if (chrs[index + 1] == '}')
            {
                index++;
                currList->add(ConsoleChar('}', fg, bg));
                if (redirectToSerial)
                    Serial::Write('}');
            }
            else if (chrs[index + 1] == 'F') // foreground color
            {
                index++;
                if (chrs[index + 1] == 0 || chrs[index + 2] == 0 || chrs[index + 3] == 0 || chrs[index + 4] == 0 || chrs[index + 5] == 0 || chrs[index + 6] == 0)
                {
                    currList->add(ConsoleChar('?', fg, bg));
                    if (redirectToSerial)
                        Serial::Write('?');
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
                    if (redirectToSerial)
                        Serial::Write('?');
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
                if (redirectToSerial)
                    Serial::Write(chrs[index]);
            }
        }
        else
        {
            currList->add(ConsoleChar(chrs[index], fg, bg));
            if (redirectToSerial)
                Serial::Write(chrs[index]);
        }
        
        //currList->add(ConsoleChar(str[i], fg, bg));
    }
    
    RemoveFromStack();
}










void ClearCharArr(ConsoleChar* charArr, int sizeX, int sizeY, uint32_t fg, uint32_t bg)
{
    ConsoleChar tempChar = ConsoleChar(' ', fg, bg);
    for (int i = 0; i < sizeX * sizeY; i++)
        charArr[i] = tempChar;
}

void ReInitCharArrWithSize(ConsoleChar** charArr, int sizeX, int sizeY, uint32_t fg, uint32_t bg)
{
    *charArr = (ConsoleChar*)_Malloc((sizeX * sizeY * sizeof(ConsoleChar)));
    ClearCharArr(*charArr, sizeX, sizeY, fg, bg);
}


NewTerminalInstance::NewTerminalInstance()
{
    AddToStack();
    redirectToSerial = false;
    //textData = List<List<ConsoleChar>*>(4);
    instanceType = InstanceType::NewTerminal;
    RenderCalled = true;
    //this->currentUser = user;
    this->window = NULL;
    backgroundColor = Colors.black;
    foregroundColor = Colors.white;
    scrollX = 0;
    scrollY = 0;
    oldScrollX = 0;
    oldScrollY = 0;

    oldWidth = 80;
    oldHeight = 160;
    //tempPixels = (ConsoleChar*)malloc(((window->size.width/8)*(window->size.height/16) * sizeof(ConsoleChar)));
    ReInitCharArrWithSize(&tempPixels, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
    ReInitCharArrWithSize(&tempPixels2, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);

    FreeFuncHelp = (void*)this;
    FreeFunc = (void(*)(void*))&Free;

    AddNewLine();
    RemoveFromStack();
}
void NewTerminalInstance::SetWindow(Window* window)
{
    this->window = window;
    if (window != NULL)
    {
        oldWidth = window->size.width;
        oldHeight = window->size.height;
        //tempPixels = (ConsoleChar*)malloc(((window->size.width/8)*(window->size.height/16) * sizeof(ConsoleChar)));
        _Free(tempPixels);
        _Free(tempPixels2);
        ReInitCharArrWithSize(&tempPixels, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
        ReInitCharArrWithSize(&tempPixels2, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
    }
    else
    {
        oldWidth = 80;
        oldHeight = 160;
        //tempPixels = (ConsoleChar*)malloc(((window->size.width/8)*(window->size.height/16) * sizeof(ConsoleChar)));
        _Free(tempPixels);
        _Free(tempPixels2);
        ReInitCharArrWithSize(&tempPixels, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
        ReInitCharArrWithSize(&tempPixels2, (oldWidth/8), (oldHeight/16), foregroundColor, backgroundColor);
    }
    Clear();
    window->brenderer->Clear(backgroundColor);
    window->renderer->Clear(backgroundColor);
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
    ClearCharArr(tempPixels2, sizeX, sizeY, foregroundColor, backgroundColor);

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
                // if (((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) < 0) || ((dx - dx1) + ((dy - dy1) * (oldWidth / 8)) > (oldWidth / 8)*(oldHeight / 16)))
                //         Panic("OUT OF BOUNDS OMGGGGGGGGGGGGGGGGGGG!");


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




    // for (int y = 0; y < sizeY; y++)
    //     for (int x = 0; x < sizeX; x++)
    //         if (tempPixels[x + (y * sizeX)] != tempPixels2[x + (y * sizeX)])
    //         {
    //             ConsoleChar chr = tempPixels[x + (y * sizeX)];
    //             // if (chr.chr == ' ')
    //             //    chr.chr = '?';
    //             // chr.fg = Colors.yellow;
    //             tempPixels2[x + (y * sizeX)] = chr;
    //             window->renderer->putChar(chr.chr, x*8 - (scrollX % 8), y*16 - (scrollY % 16), chr.fg, chr.bg);
    //         }


    //window->BlitBackbuffer();
    RemoveFromStack();
}

void NewTerminalInstance::Render()
{
    RenderCalled = true;
}

void NewTerminalInstance::Reload()
{
    oldScrollX = scrollX + 1;
    Render();
}

void NewTerminalInstance::DoRender()
{
    if (scrollX == oldScrollX && scrollY == oldScrollY && oldHeight == window->size.height && oldWidth == window->size.width)
        if (!RenderCalled)
            return;
    RenderCalled = false;

    AddToStack();
    if (window == NULL)
        Panic("Trying to Render with window being NULL!", true);


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
            _Free(tempPixels);
            ReInitCharArrWithSize(&tempPixels, (window->size.width/8), (window->size.height/16), foregroundColor, backgroundColor);
            _Free(tempPixels2);
            ReInitCharArrWithSize(&tempPixels2, (window->size.width/8), (window->size.height/16), foregroundColor, backgroundColor);
        }

        window->renderer->Clear(backgroundColor);
        ClearCharArr(tempPixels, (window->size.width/8), (window->size.height/16), foregroundColor, backgroundColor);
        if (textData.getCount() == 0)
        {
            //window->BlitBackbuffer();
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
                        Panic("OUT OF BOUNDS OMGGGGGGGGGGGGGGGGGGG!", true);
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
    if (window == NULL)
        Panic("Trying to clear window which is NULL!", true);
    AddToStack();
    //window->brenderer->Clear(backgroundColor);
    //window->renderer->Clear(backgroundColor);
    //ClearCharArr(tempPixels,  (oldWidth / 8), (oldHeight / 16), foregroundColor, backgroundColor);
    //ClearCharArr(tempPixels2, (oldWidth / 8), (oldHeight / 16), foregroundColor, backgroundColor);
    ClearListList(&textData);
    AddNewLine();
    //Render();
    //window->BlitBackbuffer();
    RemoveFromStack();
}

void NewTerminalInstance::Free()
{
    ClearListList(&textData);
    textData.free();
    _Free(tempPixels);
    _Free(tempPixels2);
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