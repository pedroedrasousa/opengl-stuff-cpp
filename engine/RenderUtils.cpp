
#include <windows.h>

#include "RenderUtils.h"
#include "gl\gl.h"
#include "Vector.h"

void RenderQuad(int x1, int x2, int y1, int y2)
{
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x1, y2);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(x2, y2);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x1, y1);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(x2, y1);
	glEnd();
}