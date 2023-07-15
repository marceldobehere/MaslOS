#include "openFileExplorer.h"
#include "../../cStdLib/cstrTools.h"
#include "../../memory/heap.h"
#include "../../OSDATA/osdata.h"
#include "../../fsStuff/fsStuff.h"
#include "../../rnd/rnd.h"
#include "../../tasks/closeWindow/taskWindowClose.h"



namespace SysApps
{
    OpenFileExplorer::OpenFileExplorer()
    {
        explorer = new Explorer();
        window = explorer->window;
        _Free(window->title);
        window->title = StrCopy("Select File to open");
        explorer->OnFileClickHelp = (void*)this;
        explorer->OnFileClickCallback = (void(*)(void*, const char*))(void*)&OnFileClick;


        window->OnCloseHelp = (void*)this;
        window->OnClose = (void(*)(void*, Window*))(void*)&OnExternalWindowClose;

        dataTask = NewSimpleDataTask();


    }


    void OpenFileExplorer::OnExternalWindowClose(Window* window)
    {
        //GlobalRenderer->Clear(Colors.purple);

        dataTask->data = NULL;
        dataTask->SetDone();

        Free();
    }

    void OpenFileExplorer::Free()
    {
        AddToStack();

        if (explorer != NULL)
            explorer->OnExternalWindowClose(window);
        explorer = NULL;
        
        _Free(this);
        RemoveFromStack();
    }


    void OpenFileExplorer::OnFileClick(const char* path)
    {
        AddToStack();

        dataTask->data = (void*)StrCopy(path);
        dataTask->SetDone();

        window->OnCloseHelp = (void*)explorer;
        window->OnClose = (void(*)(void*, Window*))(void*)&Explorer::OnExternalWindowClose;
        osData.osTasks.add(NewWindowCloseTask(window));

        _Free(this);
        RemoveFromStack();
    }




}