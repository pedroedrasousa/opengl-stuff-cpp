/*
#include "Renderer.h"
#include "OpenGLExt.h"

GLenum getPixelDataFormat(const Image &image)
{
	if(image.getFormat() == IFORMAT_BGR)
		return GL_BGR;

	if(image.getFormat() == IFORMAT_BGRA)
		return GL_BGRA;

	switch( image.GetNumChannels() )
	{
	case 1:
		return GL_LUMINANCE;
	case 2:
		return GL_LUMINANCE_ALPHA;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	default:
		return 0;
	}
}

GLenum getInternalFormat(const Image &image)
{
	switch( image.getFormat() )
	{
	case IFORMAT_NONE:
		return 0;
	case IFORMAT_RGB:
	case IFORMAT_BGR:
		return GL_RGB8;
	case IFORMAT_RGBA:
	case IFORMAT_BGRA:
		return GL_RGBA8;
	case IFORMAT_DXT1:
		return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	case IFORMAT_DXT3:
		return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case IFORMAT_DXT5:
		return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	default:
		return 0;
	}
}

GLuint Renderer::LoadTexture(Image &image) const {
	GLuint texture = 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	unsigned int format = getPixelDataFormat(image);
	int internalFormat = getInternalFormat(image);

	if( !image.isCompressed() )
	{
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
		//if (flags & TEX_CLAMP)
		//{
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//}

		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		//glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, image.channels, image.width, image.height, format, GL_UNSIGNED_BYTE, image.pixels);

		image.CreateMipMaps();

		for(int i = 0; i < image.GetNumMipMaps(); i++)
		{
			// Get offset
			int offset = 0;
			for(int j = 0; j < i; j++)
				offset += (image.GetWidth() >> (j)) * (image.GetHeight() >> (j)) * image.GetNumChannels();

			glTexImage2D(GL_TEXTURE_2D, i, internalFormat, image.GetWidth() >> i, image.GetHeight() >> i, 0, format, GL_UNSIGNED_BYTE, image.GetPixels() + offset);
		}
	}
	else
	{
		int width = image.GetWidth(), height = image.GetHeight();
		int size, offset = 0;
		int blockSize = (image.getFormat() == IFORMAT_DXT1) ? 8 : 16;

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);
		for(int i = 0; i < image.GetNumMipMaps(); i++)
		{
			size = ( (width+3) / 4 ) * ( (height+3) / 4 ) * blockSize;
			glCompressedTexImage2DARB(GL_TEXTURE_2D, i, internalFormat, width, height, 0, size, image.GetPixels() + offset);
		
			offset += size;
            width  >>= 1;
            height >>= 1;
		}
	}

	return texture;
}

GLuint Renderer::LoadTexture(const char *filename) const
{
	Image image;
	image.Load(filename);

	return LoadTexture(image);
}*/