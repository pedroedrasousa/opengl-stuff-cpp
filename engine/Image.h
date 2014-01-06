
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <assert.h>

#pragma pack (push, 1)

struct BmpHeader {
	WORD    bfType; 
	DWORD   bfSize; 
	WORD    bfReserved1; 
	WORD    bfReserved2; 
	DWORD   bfOffBits; 
	DWORD	biSize;
	DWORD	width;
	DWORD	height;
	WORD	biPlanes;
	WORD	bpp;
	DWORD	biCompression;
	DWORD	biSizeImage;
	DWORD	biXPelsPerMeter;
	DWORD	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
};

struct DDSHeader {
	DWORD	ddsIdentifier;
	BYTE	junk[8];
	DWORD	height;
	DWORD	width;
	BYTE	junk2[8];
	DWORD	numMipMaps;
	BYTE	junk3[52];
	DWORD	fourCC;
	DWORD	bpp;
	BYTE	junk4[36];
};

struct TgaHeader {
	BYTE	descriptionlen;
	BYTE	cmaptype;
	BYTE	imagetype;
	WORD	cmapstart;
	WORD	cmapentries;
	BYTE	cmapbits;
	WORD	xoffset;
	WORD	yoffset;
	WORD	width;
	WORD	height;
	BYTE	bpp;
	BYTE	attrib;
};

#pragma pack (pop)

enum ResizeMethod {
	IMAGE_NEAREST,
	IMAGE_BICUBIC
};

typedef enum imageFormat {
	IFORMAT_NONE	= 0,
	IFORMAT_I		= 1,
	IFORMAT_RGB		= 2,
	IFORMAT_RGBA	= 3,
	IFORMAT_BGR		= 4,
	IFORMAT_BGRA	= 5,
	IFORMAT_DXT1	= 6,
	IFORMAT_DXT3	= 7,
	IFORMAT_DXT5	= 8
};

//====================================================================================================
// Image
//====================================================================================================

class Image
{
public:

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetNumChannels() const{ return channels; }
	unsigned const char *GetPixels() const{ return pixels; }
	unsigned int getFormat() const { return format; }
	int GetNumMipMaps() const { return numMipMaps; }
	bool isCompressed() const { return format >= IFORMAT_DXT1; }

	void SetWidth(int width)				{ this->width = width;			}
	void SetHeight(int height)				{ this->height = height;		}
	void SetNumChannels(int numChannels)	{ this->channels = numChannels;	}
	void SetPixels(unsigned char *pixels)	{ this->pixels = pixels;		}
	void SetFormat(unsigned int format)		{ this->format = format;		}

	Image();
	~Image();

	bool Create(int width, int height, int channels, unsigned int format, unsigned char *pixels);
	bool Load(const char *filename);
	bool Resize(int newWidth, int newHeight, ResizeMethod method = IMAGE_BICUBIC);
	bool CreateMipMaps();

	bool FlipVertical();
	bool FlipChannels();

	bool ToNormalMap(float scale = 1);
	bool ToGrayScale();

private:

	int width;
	int height;
	int channels;
	unsigned char *pixels;
	unsigned int format;
	int numMipMaps;

	bool LoadBMP(const char *filename);
	bool LoadDDS(const char *filename);
	bool LoadJPEG(const char *filename);
	bool LoadPNG(const char *filename);
	bool LoadTGA(const char *filename);

	unsigned char *ResizeBicubic(int newWidth, int newHeight);
	unsigned char *ResizeNearestNeighbour(int newWidth, int newHeight);
};

#endif /*__IMAGE_H__*/