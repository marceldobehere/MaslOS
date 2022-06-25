#pragma once

#include "FileStruct.h"
#include "ImageStruct.h"
#include "../../memory/heap.h"
#include "../../Rendering/Framebuffer.h"

namespace kernelFiles
{
    ImageFile* ConvertFileToImage(DefaultFile* File);
	Framebuffer* ConvertImageToFramebuffer(ImageFile* image);
}


/*
EFI_FILE* img = LoadFile(Directory, Path, ImageHandle, SystemTable);

	if (img == NULL)
		return NULL;

	ImageFile* image;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(ImageFile), (void**)&image);
	
	// PSF1_HEADER* fontHeader;
	// SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	 
	UINTN size = 3*4;
	int32_t arr[3];
	img->Read(img, &size, arr);

	// int32_t arr2[3] = {0, 0, 0};

	// for (int i = 0; i < 3; i++)
	// 	arr2[i] = (((uint32_t)arr[(i*4)+3]) << 24) | (((uint32_t)arr[(i*4)+2]) << 16) | (((uint32_t)arr[(i*4)+1]) << 8) | (((uint32_t)arr[(i*4)+0]) << 0);

	Print(L"Image Data:\n\r");
	Print(L" - Width:  %d\n\r", arr[0]);
	Print(L" - Height: %d\n\r", arr[1]);
	Print(L" - Size:   %d\n\r", arr[2]);

	image->width = arr[0];
	image->height = arr[1];


	{
		img->SetPosition(img, 4*3);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, arr[2], (void**)&image->imageBuffer);
		size = arr[2];
		img->Read(img, &size, (char*)image->imageBuffer);
	}

	// size = 10; //(UINTN)(arr[2] * 0 + 100);
	// char data[arr[2]];
	// img->Read(img, &size, data);
	//Print(L" - First Byte:   %d\n\r", *((char*)image->imageBuffer));
	return image;


*/