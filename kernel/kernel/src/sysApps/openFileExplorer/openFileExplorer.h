#pragma once
#include "../explorer/explorer.h"
#include "../../tasks/simpleData/taskSimpleData.h"

namespace SysApps
{
    class OpenFileExplorer
    {
        private:
        Window* window;
        Explorer* explorer;
       
        
        public:
        TaskSimpleData* dataTask;
        OpenFileExplorer();

        void OnExternalWindowClose(Window* window);

        void OnFileClick(const char* path);

        void Free();
    };



}