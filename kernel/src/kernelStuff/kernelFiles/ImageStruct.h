#pragma once
#include <stdint.h>

namespace kernelFiles
{
	typedef struct
	{
		int32_t width, height, xOff, yOff;
		int64_t size;
		void* imageBuffer;
	} ImageFile;

}