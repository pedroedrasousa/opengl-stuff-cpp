#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <windows.h>
#include "gl\gl.h"
#include "gl\glext.h"
#include "image.h"

struct CubeMapFilePath {
	char posX[64];
	char negX[64];
	char posY[64];
	char negY[64];
	char posZ[64];
	char negZ[64];
};

GLenum getPixelDataFormat(const Image &image);
GLenum getInternalFormat(const Image &image);

class Texture
{
public:

    Texture();
   ~Texture();

	GLuint Texture::GetID() const		{ return m_id;		}
	int Texture::getWidth() const		{ return m_width;		}
	int Texture::getHeight() const		{ return m_height;	}
	int Texture::getChannels() const	{ return m_channels;	} 

	void Activate(int unit);
	void Deactivate();
	void Bind(int unit);
	bool LoadImage(Image &image, GLuint target, bool mipmap);
	bool Load1D(Image &image, GLuint clampS, GLuint magFilter, GLuint minFilter);
	bool Load2D(const char *filename);
	bool Load2D(Image &image, GLuint clampS, GLuint clampT, GLuint magFilter, GLuint minFilter, bool mipmap);
	bool LoadCubeMap(CubeMapFilePath filePath, GLuint clampS, GLuint clampT, GLuint magFilter, GLuint minFilter, bool mipmap);
	bool GenerateNormalizationCubeMap(GLuint magFilter, GLuint minFilter, int size, bool mipap);
	bool CreateRenderTarget(const int width, const int height, const int channels, const int type, bool clampToEdge);
	bool CreateRenderTargetRect(const int width, const int height, const int channels, const int type, bool clampToEdge);

private:

	GLuint	m_id;
	int		m_unit;
	GLuint	m_target;
	int		m_width;
	int		m_height;
	int		m_channels;
};
#endif /*__TEXTURE_H__*/