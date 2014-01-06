
#include <assert.h>

#include "Texture.h"
#include "OpenGLExt.h"
#include "NormalizationCubeMap.h"

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
	case IFORMAT_I:
		return GL_LUMINANCE;
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

Texture::Texture()
{
	m_id		= 0;
	m_unit		= -1;
	m_target	= 0;
	m_width		= 0;
	m_height	= 0;
	m_channels	= 0;
}

Texture::~Texture()
{

}

void Texture::Activate(int unit)
{
	assert(m_id >= 0 && unit >= 0);

	glActiveTextureARB(GL_TEXTURE0_ARB + unit);

	glEnable(m_target);
	glBindTexture(m_target, m_id);

	m_unit = unit;
}

void Texture::Deactivate()
{
	assert(m_id >= 0 && m_unit >= 0);

	glActiveTextureARB(GL_TEXTURE0_ARB + m_unit);

	glDisable(m_target);
}

void Texture::Bind(int unit)
{
	assert(m_id >= 0 && unit >= 0);

	glActiveTextureARB(GL_TEXTURE0_ARB + unit);
	glBindTexture(m_target, m_id);

	m_unit = unit;
}

bool Texture::LoadImage(Image &image, GLuint target, bool mipmap)
{
	GLuint format		= getPixelDataFormat(image);
	int internalFormat	= getInternalFormat(image);

	if( !image.isCompressed() )
	{
		if(mipmap)
		{
			glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

			image.CreateMipMaps();

			for(int i = 0; i < image.GetNumMipMaps(); i++)
			{
				// Get offset
				int offset = 0;
				for(int j = 0; j < i; j++)
					offset += (image.GetWidth() >> (j)) * (image.GetHeight() >> (j)) * image.GetNumChannels();

				glTexImage2D(target, i, internalFormat, image.GetWidth() >> i, image.GetHeight() >> i, 0, format, GL_UNSIGNED_BYTE, image.GetPixels() + offset);
			}
		}
		else
			glTexImage2D(target, 0, internalFormat, image.GetWidth(), image.GetHeight(), 0, format, GL_UNSIGNED_BYTE, image.GetPixels());
	}
	else
	{
		int width = image.GetWidth(), height = image.GetHeight();
		int size, offset = 0;
		int blockSize = (image.getFormat() == IFORMAT_DXT1) ? 8 : 16;

		for(int i = 0; i < image.GetNumMipMaps(); i++)
		{
			size = ( (width+3) / 4 ) * ( (height+3) / 4 ) * blockSize;
			glCompressedTexImage2DARB(target, i, internalFormat, width, height, 0, size, image.GetPixels() + offset);
		
			offset += size;
            width  >>= 1;
            height >>= 1;
		}
	}

	return true;
}

bool Texture::Load1D(Image &image, GLuint clampS, GLuint magFilter, GLuint minFilter)
{
	m_target = GL_TEXTURE_1D;

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_1D, m_id);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, clampS);
	//glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, clampT);

	m_height	= image.GetHeight();
	m_width		= image.GetWidth();
	m_channels  = image.GetNumChannels();

	GLuint format		= getPixelDataFormat(image);
	int internalFormat	= getInternalFormat(image);

	glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, image.GetWidth(), 0, format, GL_UNSIGNED_BYTE, image.GetPixels());
	
	return true;

	//return LoadImage(image, GL_TEXTURE_2D, mipmap);
}

bool Texture::Load2D(const char *filename)
{
	Image image;

	image.Load(filename);

	return Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
}

bool Texture::Load2D(Image &image, GLuint clampS, GLuint clampT, GLuint magFilter, GLuint minFilter, bool mipmap)
{
	m_target = GL_TEXTURE_2D;

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampT);

	m_height	= image.GetHeight();
	m_width		= image.GetWidth();
	m_channels  = image.GetNumChannels();

	return LoadImage(image, GL_TEXTURE_2D, mipmap);
}

bool Texture::LoadCubeMap(CubeMapFilePath filePath, GLuint clampS, GLuint clampT, GLuint magFilter, GLuint minFilter, bool mipmap)
{
	Image image;

	m_target = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_id);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, magFilter);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, minFilter);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, clampS);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, clampT);

	image.Load(filePath.posX);
	if( !LoadImage(image, GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, mipmap) )
		return false;

	image.Load(filePath.negX);
	if( !LoadImage(image, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, mipmap) )
		return false;

	image.Load(filePath.posY);
	if( !LoadImage(image, GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, mipmap) )
		return false;

	image.Load(filePath.negY);
	if( !LoadImage(image, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, mipmap) )
		return false;

	image.Load(filePath.posZ);
	if( !LoadImage(image, GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, mipmap) )
		return false;

	image.Load(filePath.negZ);
	if( !LoadImage(image, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, mipmap) )
		return false;

	m_height	= image.GetHeight();
	m_width		= image.GetWidth();
	m_channels  = image.GetNumChannels();


	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

bool Texture::GenerateNormalizationCubeMap(GLuint magFilter, GLuint minFilter, int size, bool mipmap)
{
	m_target = GL_TEXTURE_CUBE_MAP;

	Image posX, negX, posY, negY, posZ, negZ;

	GenerateNormalisationCubeMap(posX, negX, posY, negY, posZ, negZ, size);

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if( !LoadImage(posX, GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, mipmap) )
		return false;

	if( !LoadImage(negX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, mipmap) )
		return false;

	if( !LoadImage(posY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, mipmap) )
		return false;

	if( !LoadImage(negY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, mipmap) )
		return false;

	if( !LoadImage(posZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, mipmap) )
		return false;

	if( !LoadImage(negZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, mipmap) )
		return false;

	m_height	= size;
	m_width		= size;
	m_channels	= 3;

	return true;
}

bool Texture::CreateRenderTarget(const int width, const int height, const int channels, const int internalFormat, bool clampToEdge)
{
	void *pixels;
	GLenum type;
	GLenum format;

	switch(internalFormat)
	{
	case GL_RGB16F_ARB:
	case GL_RGB32F_ARB:
	//case GL_RGB_FLOAT16_ATI:
	//case GL_RGB_FLOAT32_ATI:
		type = GL_FLOAT;
		pixels = new float[width * height * channels];
		memset(pixels, 0, width * height * channels * sizeof(float));
		break;
	default:
		type = GL_UNSIGNED_BYTE;
		pixels = new BYTE[width * height * channels];
		memset(pixels, 0, width * height * channels);
	}

	switch(channels)
	{
	case 1:
		format = GL_LUMINANCE;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}

	m_target	= GL_TEXTURE_2D;
	m_width		= width;
	m_height	= height;
	m_channels  = channels;

	glGenTextures(1, &m_id);								
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if(clampToEdge)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	
	delete [] pixels;	

	return true;
}
/*
bool Texture::CreateRenderTargetRect(const int width, const int height, const int channels, const int type, bool clampToEdge)
{
	m_target	= GL_TEXTURE_2D;
	m_width		= width;
	m_height	= height;
	m_channels  = channels;

	BYTE *pixels;
	pixels = new BYTE [width * height * channels];
	memset(pixels, 0, width * height * channels);

	glGenTextures(1, &m_id);								
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_id);

	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, type, width, height,0, type, GL_UNSIGNED_BYTE, pixels);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if(clampToEdge)
	{
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	delete [] pixels;	

	return true;
}
*/