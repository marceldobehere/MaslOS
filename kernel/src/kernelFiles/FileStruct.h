#pragma once
#include <stdint.h>

namespace kernelFiles
{
	typedef struct
	{
        int64_t size;
        int32_t filenameSize;
        
        char* filename;
        void* fileData;
	} DefaultFile;

}