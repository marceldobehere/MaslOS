#pragma once


#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04


struct PSF1_HEADER
{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
};

struct PSF1_FONT
{
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
};

