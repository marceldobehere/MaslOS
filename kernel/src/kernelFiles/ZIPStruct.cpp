#include "ZIPStruct.h"
#include "../cmdParsing/cstrTools.h"
#include <stddef.h>
#include "../OSDATA/MStack/MStackM.h"

namespace kernelFiles
{
    namespace ZIP
    {
        DefaultFile* GetFileFromFileName(ZIPFile* zip, const char* filename)
        {
            AddToStack("GetFileFromFileName", "kernelFiles/ZIPStruct.cpp");
            if (zip == NULL)
            {
                RemoveFromStack();
                return NULL;
            }
            
            
            for (int i = 0; i < zip->fileCount; i++)
            {
                if (StrEquals(zip->files[i].filename, filename))
                {
                    RemoveFromStack();
                    return &zip->files[i];
                }
            }

            RemoveFromStack();
            return NULL;
        }
    }
}