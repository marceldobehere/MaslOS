#pragma once
#include "../explorer/explorer.h"
#include "../../tasks/simpleData/taskSimpleData.h"

namespace SysApps
{
    class SaveFileExplorer
    {
        private:
        Window* window;
        Explorer* explorer;
       
        
        public:
        TaskSimpleData* dataTask;
        SaveFileExplorer();

        void OnExternalWindowClose(Window* window);

        void OnFileClick(const char* path);

        void Free();
    };



}