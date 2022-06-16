#pragma once
#include <stdint.h>

namespace kernelFiles
{
	typedef struct
	{
		int64_t width, height;
		void* imageBuffer;
	} ImageFile;

}