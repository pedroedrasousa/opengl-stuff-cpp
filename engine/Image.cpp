
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "zlib.lib")

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>

#include "imaging\\jpeglib.h"
#include "imaging\\png.h"
#include "math\\Matrix.h"
#include "Image.h"

Image::Image()
{
	width = 0;
	height = 0;
	channels = 0;
	pixels = NULL;
	format = IFORMAT_NONE;
	numMipMaps = 1;
}

Image::~Image()
{
	if(pixels != NULL)
	{
		free(pixels);
	}
}

//====================================================================================================
// BMP
//====================================================================================================

bool Image::LoadBMP(const char *filename)
{
	BmpHeader header;

	FILE *file;
	unsigned char *dest, *src, *tmp;
	int i;
	unsigned char palette[1024];

	if( fopen_s(&file, filename, "rb") != 0 ) return false;
	
	// Read the header
	fread(&header, sizeof(BmpHeader), 1, file);

	if( memcmp(&header.bfType, "BM", 2) )
	{
		fclose(file);
		return false;
	}

	width    = header.width;
	height   = header.height;
	channels = (header.bpp == 32)? 4 : 3;
	
	if(header.bpp == 24 || header.bpp == 32)
	{
		format = (channels == 3)? IFORMAT_BGR : IFORMAT_BGRA;
		pixels = new unsigned char[width * height * channels];
		for(i = 0; i < height; i++)
		{
			dest = pixels + i * width * channels;
			fread(dest, width * channels, 1, file);
		}
	}
	else
	{
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}

//====================================================================================================
// DDS
//====================================================================================================

bool Image::LoadDDS(const char *filename)
{
	DDSHeader header;
	FILE *file;
	int size;
	
	if( fopen_s(&file, filename, "rb") != 0 )
		return false;

	// Find file size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
		
	// Read the header
	fread(&header, sizeof(DDSHeader), 1, file);

	if( memcmp(&header.ddsIdentifier, "DDS", 3) )
	{
		fclose(file);
		return false;
	}

	width    = header.width;
	height   = header.height;
	numMipMaps = header.numMipMaps;

	if( !memcmp(&header.fourCC, "DXT1", 4) )
	{
		format = IFORMAT_DXT1;
		channels = 3;
	}
	else if( !memcmp(&header.fourCC, "DXT3", 4) )
	{
		format = IFORMAT_DXT3;
		channels = 4;
	}
	else if( !memcmp(&header.fourCC, "DXT5", 4) )
	{
		format = IFORMAT_DXT5;
		channels = 4;
	}
	else
	{
		fclose(file);
		return false;
	}

	// Read the image data
	size -= sizeof(DDSHeader);
	pixels = new unsigned char[size];
	fread(pixels, 1, size, file);

	fclose(file);
	
	return true;
}

//====================================================================================================
// JPEG
//====================================================================================================

bool Image::LoadJPEG(const char *filename)
{
	FILE *file;
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	if( fopen_s(&file, filename, "rb") != 0 )
		return false;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, file);
	jpeg_read_header(&cinfo, true);
	jpeg_start_decompress(&cinfo);
    
	switch(cinfo.num_components)
	{
	case 1:
		format = IFORMAT_I;
		break;
	case 3:
		format = IFORMAT_RGB;
		break;
	}	
	
	width     = cinfo.output_width;
	height    = cinfo.output_height;
	channels  = cinfo.num_components;

	pixels = new unsigned char[width * height * channels];
	unsigned char *curr_scanline = pixels;

	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &curr_scanline, 1);
		curr_scanline += width * cinfo.num_components;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(file);

	return true;
}

//====================================================================================================
// PNG
//====================================================================================================

bool Image::LoadPNG(const char *filename)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr  = NULL;
    FILE *file;

    png_byte pbSig[8];
    int iBitDepth, iColorType;
    double dGamma;
    png_color_16 *pBackground;
    png_byte **ppbRowPointers;

    // open the PNG input file
    if( fopen_s(&file, filename, "rb") != 0 )
		return false;

    // first check the eight byte PNG signature
    fread(pbSig, 1, 8, file);
    if( !png_check_sig(pbSig, 8) )
	{
		fclose(file);
		return false;
	}

    // create the two png(-info) structures
    if( (png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr) NULL, (png_error_ptr) NULL)) == NULL )
	{
		fclose(file);
        return false;
    }

    if( ( info_ptr = png_create_info_struct(png_ptr) ) == NULL )
	{
        png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(file);
        return false;
    }

	// initialize the png structure
	png_init_io(png_ptr, file);
	png_set_sig_bytes(png_ptr, 8);
	
	// read all PNG info up to image pixels
	png_read_info(png_ptr, info_ptr);
	
	// get width, height, bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) &width, (png_uint_32 *) &height, &iBitDepth, &iColorType, NULL, NULL, NULL);
	
	// expand images of all color-type and bit-depth to 3x8 bit RGB images
	// let the library process things like alpha, transparency, background
	if (iBitDepth == 16) png_set_strip_16(png_ptr);
	if (iColorType == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (iBitDepth < 8) png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
	
	// set the background color to draw transparent and alpha images over.
	if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
		png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
	
	// if required set gamma conversion
	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, 2.2, dGamma);

	//if (!useRGBA)
		//png_set_bgr(png_ptr);
	
	// after the transformations have been registered update info_ptr pixels
	png_read_update_info(png_ptr, info_ptr);
	
	// get again width, height and the new bit-depth and color-type
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) &width, (png_uint_32 *) &height, &iBitDepth, &iColorType, NULL, NULL, NULL);
	channels = png_get_channels(png_ptr, info_ptr);

	switch(channels)
	{
		case 1:
			format = IFORMAT_I;
			break;
		case 3:
			format = IFORMAT_RGB;
			break;
		case 4:
			format = IFORMAT_RGBA;
			break;
	}
	
	// now we can allocate memory to store the image
	pixels = new unsigned char[width * height * channels];
	
	// set the individual row-pointers to point at the correct offsets
	ppbRowPointers = new png_bytep[height];
	for (int i = 0; i < height; i++)
		ppbRowPointers[i] = pixels + i * width * channels;
	
	// now we can go ahead and just read the whole image
	png_read_image(png_ptr, ppbRowPointers);
	
	// read the additional chunks in the PNG file (not really needed)
	png_read_end(png_ptr, NULL);
	
	delete ppbRowPointers;

	// and we're done
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(file);
	
    return true;
}

//====================================================================================================
// TGA
//====================================================================================================

bool Image::LoadTGA(const char *filename)
{
	FILE *file;
	TgaHeader header;

	int size, x, y, palLength;
	unsigned char *tempBuffer, *fBuffer, *dest, *src;
	unsigned int tempPixel;
	unsigned char palette[768];

	if( fopen_s(&file, filename, "rb") != 0 )
		return false;
	
	// Find file size
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Read the header
	fread(&header, sizeof(TgaHeader), 1, file);
	
	width  = header.width;
	height = header.height;
	channels = header.bpp / 8;

	// Read the palette if present
	if( (palLength = header.descriptionlen + header.cmapentries * header.cmapbits / 8) > 0 )
		fread(palette, sizeof(palette), 1, file);
	
	// Read the file pixels
	fBuffer = new unsigned char[size - sizeof(header) - palLength];
	fread(fBuffer, size - sizeof(header) - palLength, 1, file);
	fclose(file);

	size = width * height * channels;

	tempBuffer = new unsigned char[size];

	// Decode if rle compressed. Bit 3 of .imagetype tells if the file is compressed
	if (header.imagetype & 0x08)
	{
		unsigned int c, count;

		dest = tempBuffer;
		src = fBuffer;
		
		while (size > 0)
		{
			// Get packet header
			c = *src++;

			count = (c & 0x7f) + 1;
			size -= count * channels;
			
			if (c & 0x80) {
				// Rle packet
				do {
					memcpy(dest,src,channels);
					dest += channels;
				} while (--count);
				src += channels;
			} else {
				// Raw packet
				count *= channels;
				memcpy(dest,src,count);
				src += count;
				dest += count;
			}
		}
		
		src = tempBuffer;
	}
	else
	{
		src = fBuffer;
	}

	src += channels * width * (height - 1);	

	int red, blue;

	//if (useRGBA){
		red  = 2;
		blue = 0;
	//} else {
		//red  = 0;
		//blue = 2;
	//}

	switch(header.bpp)
	{
	case 8:
		format = IFORMAT_RGB;
		dest = pixels = new unsigned char[width * height * 3];
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				tempPixel = 3 * (*src++);
				*dest++ = palette[tempPixel + red];
				*dest++ = palette[tempPixel + 1];
				*dest++ = palette[tempPixel + blue];
			}
			src -= 2 * width;
		}
		break;
	case 16:
		format = IFORMAT_RGBA;
		dest = pixels = new unsigned char[width * height * 4];
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				tempPixel = *((unsigned short *) src);

				dest[blue] = ((tempPixel >> 10) & 0x1F) << 3;
				dest[1]    = ((tempPixel >>  5) & 0x1F) << 3;
				dest[red]  = ((tempPixel      ) & 0x1F) << 3;
				dest[3]    = ((tempPixel >> 15) ? 0xFF : 0);
				dest += 4;
				src += 2;
			}
			src -= 4 * width;
		}
		break;
	case 24:
		format = IFORMAT_RGB;
		dest = pixels = new unsigned char[width * height * 3];
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				*dest++ = src[red];
				*dest++ = src[1];
				*dest++ = src[blue];
				src += 3;
			}
			src -= 6 * width;
		}
		break;
	case 32:
		format = IFORMAT_RGBA;
		dest = pixels = new unsigned char[width * height * 4];
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				*dest++ = src[red];
				*dest++ = src[1];
				*dest++ = src[blue];
				*dest++ = src[3];
				src += 4;
			}
			src -= 8 * width;
		}
		break;
	}

	delete tempBuffer;
	delete fBuffer;

	return true;
}

bool Image::Create(int width, int height, int channels, unsigned int format, unsigned char *pixels)
{	
	if( width <= 0 || height <= 0 || channels <=0 )
		return false;

	if(format > IFORMAT_BGRA && format < IFORMAT_NONE)
		return false;

	this->width		= width;
	this->height	= height;
	this->channels	= channels;
	this->format	= format;
	this->pixels	= pixels;

	this->numMipMaps = 1;

	return true;
}

bool Image::Load(const char *filename)
{
	const char *ext = strrchr(filename, '.');

	if(ext)
	{
		if (_stricmp(ext,".bmp") == 0)
		{
			if( !LoadBMP(filename) )
				return false;
		}
		else if (_stricmp(ext,".dds") == 0)
		{
			if( !LoadDDS(filename) )
				return false;
			// Can't flip verticaly, image will be upside down
		}
		else if (_stricmp(ext,".jpg") == 0)
		{
			if( !LoadJPEG(filename) )
				return false;
			FlipVertical();
		}
		else if(_stricmp(ext,".png") == 0)
		{
			if( !LoadPNG(filename) )
				return false;
			FlipVertical();
		}
		else if(_stricmp(ext,".tga") == 0)
		{
			if( !LoadTGA(filename) )
				return false;
			FlipVertical();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

float CubicWeighting(float x)
{
	float a, b, c, d;

    a = x + 2.0f;
    b = x + 1.0f;
    c = x;
    d = x - 1.0f;

    a = (a > 0.0f)? a : 0.0f;
    b = (b > 0.0f)? b : 0.0f;
    c = (c > 0.0f)? c : 0.0f;
    d = (d > 0.0f)? d : 0.0f;

	a = a * a * a;
	b = b * b * b;
	c = c * c * c;
	d = d * d * d;

	return 1.0f / 6.0f * (a - 4.0f * b + 6.0f * c - 4.0f * d);
}

unsigned char *Image::ResizeNearestNeighbour(int newWidth, int newHeight)
{
	int sampleX, sampleY;
	unsigned char *newPixels, *dst;
	dst = newPixels = new unsigned char[newWidth * newHeight * channels];

	for(int y = 0; y < newHeight; y++)
	{
		sampleY = (height - 1) * y / (newHeight - 1);
		for(int x = 0; x < newWidth; x++)
		{
			sampleX = (width - 1) * x / (newWidth - 1);
			for(int i = 0; i < channels; i++)
			{
				*dst++ = *(pixels + width * sampleY * channels + sampleX * channels + i);
			}
		}
	}

	return newPixels;
}

unsigned char *Image::ResizeBicubic(int newWidth, int newHeight)
{
	int sampleX, sampleY;
	float fSampleX, fSampleY;
	unsigned char auxByte, final[4];
	unsigned char *newPixels, *dst;
	float dx, dy;
	float cw;
	int src_x;
	int src_y;

	dst = newPixels = new unsigned char[newWidth * newHeight * channels];

	for(int y = 0; y < newHeight; y++)
	{
		fSampleY = ( (float)height - 1.0f ) * (float)y / ( (float)newHeight - 1.0f );
		sampleY = (int)fSampleY;
		dy = fSampleY - sampleY;

		for(int x = 0; x < newWidth; x++)
		{
			fSampleX = ( (float)width - 1.0f ) * (float)x / ( (float)newWidth - 1.0f );
			sampleX = (int)fSampleX;
			dx = fSampleX - sampleX;

			cw = 0.0f;
			final[0] = final[1] = final[2] = final[3] = 0;

			for(int i = -1; i <= 2; i++)
			{
				for(int j = -1; j <= 2; j++)
				{
                    cw = CubicWeighting( (float)i - dx ) * CubicWeighting( dy - (float)j );

					src_x = sampleX + i;
					src_y = sampleY + j;

					if(src_x < 0) src_x = 0;
					if(src_y < 0) src_y = 0;
					if(src_x > width - 1)	src_x = width - 1;
					if(src_y > height - 1)	src_y = height - 1;

					for(int k = 0; k < channels; k++)
					{
						auxByte = *(pixels + width * src_y * channels + src_x * channels + k);
						final[k] += auxByte * cw;
					}
				}
			}

			for(int i = 0; i < channels; i++)
				*dst++ = final[i];
		}
	}

	return newPixels;
}

bool Image::Resize(int newWidth, int newHeight, const ResizeMethod method)
{
	if( format == IFORMAT_NONE || this->isCompressed() )
		return false;

	if(newWidth == width && newHeight == height)
		return true;

	unsigned char *newPixels;

	switch(method)
	{
	case IMAGE_NEAREST:
		newPixels = ResizeNearestNeighbour(newWidth, newHeight);
		break;
 
	case IMAGE_BICUBIC:
		newPixels = ResizeBicubic(newWidth, newHeight);
		break;
	}

	delete pixels;
	pixels = newPixels;
	width  = newWidth;
	height = newHeight;
	numMipMaps = 1;

	return true;
}

bool Image::CreateMipMaps()
{
	if( format == IFORMAT_NONE || this->isCompressed() )
		return false;

	unsigned char *newPixels;
	int newWidth	= width;
	int newHeight	= height;
	int mipMapSize;
	int totalSize = width * height * channels;

	numMipMaps = 1;

	for(;;)
	{
		numMipMaps++;

		newWidth	>>= 1;
		newHeight	>>= 1;

		mipMapSize = newWidth * newHeight * channels;
		totalSize += mipMapSize;

		newPixels = ResizeBicubic(newWidth, newHeight);

		pixels = (unsigned char*) realloc(pixels, totalSize);

		memcpy(pixels + totalSize - mipMapSize, newPixels, mipMapSize);

		delete [] newPixels;

		if(newWidth = 1 || newHeight == 1)
			break;
	}

	return true;
}

bool Image::FlipVertical()
{
	if( format == IFORMAT_NONE || this->isCompressed() )
		return false;

	unsigned char *tmpPixels = new unsigned char[this->width * this->height * this->channels];
	unsigned char *dest, *src;

	memcpy(tmpPixels, this->pixels, this->width * this->height * this->channels);

	for (int i = 0; i < this->height; i++)
	{
		dest = pixels + i * width * channels;
		src = tmpPixels + (this->height - i - 1) * this->width * this->channels;

		memcpy(dest, src, this->width * this->channels);
	}
	
	delete tmpPixels;

	return true;
}

bool Image::FlipChannels()
{
	if( format == IFORMAT_NONE || this->isCompressed() )
		return false;

	unsigned char *pPixel = this->pixels;
	unsigned char auxByte;

	int nPixels = this->width * this->height;

	for(int i = 0; i < nPixels; i++)
	{
		auxByte = pPixel[2];
		pPixel[2] = pPixel[0];
		pPixel[0] = auxByte;
		pPixel += this->channels;
	}
}

bool Image::ToGrayScale()
{
	if( format == IFORMAT_NONE || this->isCompressed() )
		return false;
	if(channels == 1)
		return true;

	unsigned char *newPixels, *src, *dst, auxByte[3];

	src = pixels;
	dst = newPixels = new unsigned char[width * height];;

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			auxByte[0] = *(pixels + x * channels + width * y * channels);
			auxByte[1] = *(pixels + x * channels + width * y * channels + 1);
			auxByte[2] = *(pixels + x * channels + width * y * channels + 2);

			*dst++ = BYTE((auxByte[0] + auxByte[1] + auxByte[2]) / 3.0f);
		}
	}

	delete pixels;
	pixels = newPixels;

	channels = 1;
	format   = IFORMAT_I;

	return true;
}

bool Image::ToNormalMap(float scale)
{
	/*
	===============================================================================

		Filter kernels:
	
		SobelX       SobelY
		1  0 -1     -1 -2 -1
		2  0 -2      0  0  0
		1  0 -1      1  2  1

	===============================================================================
	*/

	if( format == IFORMAT_NONE  || this->isCompressed() )
		return false;

	if(format != IFORMAT_I)
		this->ToGrayScale();

	unsigned char *newPixels, *dst, auxByte;
	float src[9];

	dst = newPixels = new unsigned char[width * height * 3];

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			int min_x = (x - 1 + width) % width;
			int max_x = (x + 1) % width;
			int min_y = (y - 1 + height) % height;
			int max_y = (y + 1) % height;

			auxByte	= *(pixels + min_x * channels + width * min_y * channels);	src[0] = float(auxByte) / 255.0f;
			auxByte	= *(pixels +	 x * channels + width * min_y * channels);	src[1] = float(auxByte) / 255.0f;
			auxByte	= *(pixels + max_x * channels + width * min_y * channels);	src[2] = float(auxByte) / 255.0f;
			auxByte	= *(pixels + min_x * channels + width *		y * channels);	src[3] = float(auxByte) / 255.0f;
			auxByte	= *(pixels +	 x * channels + width *		y * channels);	src[4] = float(auxByte) / 255.0f;
			auxByte	= *(pixels + max_x * channels + width *		y * channels);	src[5] = float(auxByte) / 255.0f;
			auxByte	= *(pixels + min_x * channels + width * max_y * channels);	src[6] = float(auxByte) / 255.0f;
			auxByte	= *(pixels +	 x * channels + width * max_y * channels);	src[7] = float(auxByte) / 255.0f;
			auxByte	= *(pixels + max_x * channels + width * max_y * channels);	src[8] = float(auxByte) / 255.0f;

			float sobelX = src[0] - src[2] + 2*src[3] - 2*src[5] + src[6] - src[8];
			float sobelY = -src[0] - 2*src[1] - src[2] + src[6] + 2*src[7] + src[8];

			sobelX *= scale;
			sobelY *= scale;

			Vec3 normal = Normalize( Vec3(sobelX, sobelY, 1.0f) );

			*dst++ = (unsigned char) ( (normal.x + 1.0f) / 2.0f * 255.0f );
			*dst++ = (unsigned char) ( (normal.y + 1.0f) / 2.0f * 255.0f );
			*dst++ = (unsigned char) ( (normal.z + 1.0f) / 2.0f * 255.0f );
		}
	}

	channels = 3;
	format   = IFORMAT_RGB; 

	delete pixels;
	pixels = newPixels;

	return true;
}