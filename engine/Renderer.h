
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <windows.h>
#include "gl\gl.h"
#include "gl\glext.h"
#include "image.h"

GLenum getPixelDataFormat(const Image &image);
GLenum getInternalFormat(const Image &image);

class Renderer {
public:
	GLuint LoadTexture(Image &image) const;
	GLuint LoadTexture(const char *filename) const;
};

#endif /*__RENDERER_H__*/