#include "kernelFiles.h"
#include "../../osData/MStack/MStackM.h"
#include "../../memory/heap.h"

namespace kernelFiles
{
    ImageFile* ConvertFileToImage(DefaultFile* File)
    {
        AddToStack();
        if (File == NULL)
        {
            RemoveFromStack();
            return NULL;
        }
        ImageFile* image = (ImageFile*)_Malloc(sizeof(ImageFile), "Converting Default Kernel File to Image File");

        //GlobalRenderer->Println("ADDR: {}", ConvertHexToString((uint64_t)&File->fileData), Colors.yellow);
        char* data = (char*) File->fileData;

        int32_t sizes[4] = {0, 0, 0, 0};

        for (int i = 0; i < 4; i++)
	 	    sizes[i] = *((int32_t*)&data[i*4]);
            //  (((uint32_t)data[(i*4)+3]) << 24) |
            //             (((uint32_t)data[(i*4)+2]) << 16) |
            //             (((uint32_t)data[(i*4)+1]) <<  8) |
            //             (((uint32_t)data[(i*4)+0]) <<  0) ;

        image->width = sizes[0];
        image->height = sizes[1];
        image->xOff = sizes[2];
        image->yOff = sizes[3];

        int64_t s2 = image->width * image->height;
        if (s2 < 0 || s2 >= File->size)
        {
            _Free(image);
            RemoveFromStack();
            return NULL;
        }

        image->size = *((int64_t*)&data[4*4]);

        image->imageBuffer = _Malloc(image->size, "Buffer for Image File");
        for (int i = 24; i < image->size+24; i++)
            ((char*)image->imageBuffer)[i-24] = data[i];

        RemoveFromStack();
        return image;
    }


    Framebuffer* ConvertImageToFramebuffer(ImageFile* image)
    {
        Framebuffer* fb = (Framebuffer*)_Malloc(sizeof(Framebuffer), "Converting Image to Framebuffer");

        fb->Width = image->width;
        fb->Height = image->height;
        fb->BaseAddress = image->imageBuffer;
        fb->BufferSize = image->size;
        fb->PixelsPerScanLine = image->width;


        return fb;
    }
}