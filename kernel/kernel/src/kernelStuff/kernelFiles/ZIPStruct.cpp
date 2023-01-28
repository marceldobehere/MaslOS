#include "ZIPStruct.h"
#include "../../cmdParsing/cstrTools.h"
#include <stddef.h>
#include "../../OSDATA/MStack/MStackM.h"

namespace kernelFiles
{
    namespace ZIP
    {
        DefaultFile* GetFileFromFileName(ZIPFile* zip, const char* filename)
        {
            AddToStack();
            if (zip == NULL)
            {
                RemoveFromStack();
                return NULL;
            }
            RemoveFromStack();




            AddToStack();

            //GlobalRenderer->Println("                                     NAME ADDR: {}", ConvertHexToString((uint64_t)filename), Colors.yellow);
            //GlobalRenderer->Println("                                     NAME: \"{}\"", filename, Colors.yellow);
            //GlobalRenderer->Println("                                     ZIP FILE COUNT: {}", to_string(zip->fileCount), Colors.yellow);

            for (int i = 0; i < zip->fileCount; i++)
            {
                AddToStack();
                //GlobalRenderer->Println("                                     NAME ADDR: {}", ConvertHexToString((uint64_t)zip->files[i].filename), Colors.yellow);
                //GlobalRenderer->Println("                                     NAME: \"{}\"", zip->files[i].filename, Colors.yellow);
                RemoveFromStack();

                AddToStack();
                if (StrEquals(zip->files[i].filename, filename))
                {
                    RemoveFromStack();
                    RemoveFromStack();
                    return &zip->files[i];
                }
                RemoveFromStack();
            }

            RemoveFromStack();
            return NULL;
        }
    }
}