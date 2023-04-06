#pragma once
#include <stdint.h>
#pragma once
#include "FileStruct.h"

namespace kernelFiles
{
	typedef struct
	{
        int64_t size;
        int32_t fileCount;
        
        DefaultFile* files;
	} ZIPFile;

    namespace ZIP
    {
        DefaultFile* GetFileFromFileName(ZIPFile* zip, const char* filename);
		ZIPFile* GetZIPFromDefaultFile(DefaultFile* file);
    }
}

/*
typedef struct
{
	int64_t size;
	int32_t filenameSize;
	
	char* filename;
	void* fileData;
} NormalFile;

typedef struct
{
	int64_t size;
	int32_t fileCount;
	
	NormalFile* files;
} ZIPFile;
*/