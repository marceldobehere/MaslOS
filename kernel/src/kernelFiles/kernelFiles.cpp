#include "kernelFiles.h"
#include "../OSDATA/MStack/MStackM.h"

namespace kernelFiles
{
    ImageFile* ConvertFileToImage(DefaultFile* File)
    {
        AddToMStack(MStack("ConvertFileToImage", "kernelFiles/kernelFiles.cpp"));
        if (File == NULL)
        {
            RemoveLastMStack();
            return NULL;
        }
        ImageFile* image = (ImageFile*)malloc(sizeof(ImageFile));

        //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&File->fileData), Colors.yellow);
        char* data = (char*) File->fileData;

        int32_t sizes[3] = {0, 0, 0};

        for (int i = 0; i < 3; i++)
	 	    sizes[i] =  (((uint32_t)data[(i*4)+3]) << 24) |
                        (((uint32_t)data[(i*4)+2]) << 16) |
                        (((uint32_t)data[(i*4)+1]) <<  8) |
                        (((uint32_t)data[(i*4)+0]) <<  0) ;

        image->width = sizes[0];
        image->height = sizes[1];

        image->imageBuffer = malloc(sizes[3]);
        for (int i = 3*4; i < sizes[3]+12; i++)
            ((char*)image->imageBuffer)[i-12] = data[i];

        RemoveLastMStack();
        return image;
    }
}