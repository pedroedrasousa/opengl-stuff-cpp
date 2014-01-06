
#include <stdio.h>

#include "Font.h"
#include "Image.h"

Font::Font(const char *file)
{
	Image image;

	hAlign = FONT_HORIZ_ALIGN_LEFT;
	vAlign = FONT_VERT_ALIGN_TOP;

	if( !image.Load(file) )
	{
		char error[100];
		sprintf(error, "Unable to Load font:\n\'%s\'", file);
		ShowCursor(true);
		MessageBox(NULL, error, "Error", MB_OK | MB_ICONERROR);
		ShowCursor(false);
		return;
	}

	image.FlipVertical();

	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());

	int size = image.GetWidth();
	step = size / 16;
	    
	list_id = glGenLists(256);

	int d1 = 0;
	int d2 = 0;
    
	for(int y = 0, i = 0; y < 16; y++)
	{

		for(int x = 0; x < 16; x++, i++)
		{
			unsigned char *ptr;

			d1 = 0;
			d2 = 9999;

			// Obter largura
			for(int j = 0; j < step; j++)
			{

				ptr = (unsigned char *)image.GetPixels() + (size * y * step + x * step + j) * 4;
				int k;
				for(k = 0; k < step; k++)
				{
					if(*(ptr + 3) != 0)
					{
						if(j < d2) d2 = j;
						if(j > d1) d1 = j;
					}
					ptr += size * 4;
				}
			}
			if(d2 != 9999) width[i] = (d1 - d2 + 1);
			else width[i] = 0;

			if(width[i] > maxWidth)
				maxWidth = width[i];

			d1 = 0;
			d2 = 9999;

			// Obter altura
			for(int j = 0; j < step; j++)
			{
				ptr = (unsigned char *)image.GetPixels() + (size * y * step + x * step + j * size) * 4;
				int k;
				for(k = 0; k < step; k++)
				{
					if(*(ptr + 3) != 0)
					{
						if(j < d2) d2 = j;
						if(j > d1) d1 = j;
					}
					ptr += 4;
				}
			}
			if(d2 != 9999) height[i] = d1 - d2 + 1;
			else height[i] = 0;

			if(height[i] > maxHeight)
				maxHeight = height[i];


			hOffset[i] = 0;

			// Obter distância vazia antes do início do caracter na horizontal
			for(int j = 0; j < step; j++)
			{
				ptr = (unsigned char *)image.GetPixels() + (size * y * step + x * step + j) * 4;
				int k;
				for(k = 0; k < step; k++)
				{
					if(*(ptr + 3) != 0) break;
					ptr += size * 4;
				}
				if(k != step) break;
				hOffset[i]++;
			}

			minVertOffset = 999;

			// Obter distância vazia mínima antes do início do caracter na vertical
			for(int j = 0; j < step; j++)
			{
				ptr = (unsigned char *)image.GetPixels() + (size * y * step + x * step + j * size) * 4;
				int k;
				for(k = 0; k < step; k++)
				{
					if(*(ptr + 3) != 0) break;
					ptr += 4;
				}
				if(k != step)
				{
					if(j - 1 < minVertOffset) minVertOffset = j - 1;
					break;
				}
			}
		}
	}

	hInterval = int(maxWidth / 6);
	vInterval = maxHeight + int(maxHeight / 6);

	for(int y = 0, i = 0; y < 16; y++)
	{
		for(int x = 0; x < 16; x++, i++)
		{
			float u1 = float(x * step + hOffset[i]) / (float)size;
			float v1 = float(y * step + minVertOffset) / (float)size;

			float u2 = u1 + (float)width[i] / (float)size;
			float v2 = v1 + (float)maxHeight / (float)size;

			glNewList(list_id + i, GL_COMPILE);
				glBegin(GL_TRIANGLE_STRIP);
					glTexCoord2f(u1, v2);
					glVertex2i(0, maxHeight);		
					glTexCoord2f(u2, v2);
					glVertex2i(width[i], maxHeight);		
					glTexCoord2f(u1, v1);
					glVertex2i(0, 0);
					glTexCoord2f(u2, v1);
					glVertex2i(width[i], 0);
				glEnd();
			glEndList();
		}
	}
}

Font::~Font()
{
	glDeleteTextures(1, &tex_id);
	glDeleteLists(256, list_id);
}

void Font::Enable(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);
	
	glDisable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_id);
}

void Font::Disable()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();


	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	//glDisable(GL_BLEND);
	//glDisable(GL_TEXTURE_2D);
}

void Font::SetAlignment(int h, int v)
{
	hAlign = h;
	vAlign = v;
}

void Font::Print(int x, int y, const char *str, ...)
{
	if(str == NULL)
		return;

	char		strText[2048];
	va_list		p_Argument;

	va_start(p_Argument, str);
	vsprintf(strText, str, p_Argument);
	va_end(p_Argument);

	int len = (int)strlen(strText);

	int phraseWidth[100] = {0};
	int textHeight = 0;

	// Obter comprimento de cada frase
	if(hAlign != FONT_HORIZ_ALIGN_LEFT)
	{
		for(int i = 0, line = 0; i < len; i++)
		{
			char c = strText[i];

			if(c == ' ')
				phraseWidth[line] += int(maxWidth / 3);
			else if(c == '\t')
				phraseWidth[line] += maxWidth * 2;
			else if(c != '\n')
			{
				phraseWidth[line] += width[c];
				if(i < len - 1 && strText[i + 1] != '\n') phraseWidth[line] += hInterval;
			}
			else 
				line++;
		}
	}

	// Obter altura do texto
	if(vAlign != FONT_VERT_ALIGN_TOP)
	{
		textHeight = step;
		for(int i = 0; i < len; i++)
			if(strText[i] == '\n')
				textHeight += vInterval;
	}

	if(vAlign == FONT_VERT_ALIGN_CENTER)
		y -= int(textHeight / 2);
	else if(vAlign == FONT_VERT_ALIGN_BOTTOM)
		y -= textHeight;

    glLoadIdentity();
	glTranslatef((float)x, (float)y, 0);

	if(hAlign == FONT_HORIZ_ALIGN_CENTER)
		glTranslatef(float(-phraseWidth[0] / 2), 0, 0);
	else if(hAlign == FONT_HORIZ_ALIGN_RIGHT)
		glTranslatef(float(-phraseWidth[0]), 0, 0);

	for(int i = 0, line = 0; i < len; i++)
	{
		char c = strText[i];

		if(c == ' ')
		{
			glTranslatef(float(maxWidth / 3), 0, 0);
		}
		else if(c == '\n')
		{
			line++;
			glLoadIdentity();
			glTranslatef((float)x, float(y + vInterval * line), 0);
			
			if(hAlign == FONT_HORIZ_ALIGN_CENTER)
				glTranslatef(float(-phraseWidth[line]/2), 0, 0);
			else if(hAlign == FONT_HORIZ_ALIGN_RIGHT)
				glTranslatef(float(-phraseWidth[line]), 0, 0);
		}
		else if(c == '\t')
		{
			glTranslatef(float(maxWidth * 2), 0, 0);
		}
		else
		{
			glCallList(list_id + c);
			glTranslatef(float(width[c] + hInterval), 0, 0);
		}
	}
}

int Font::GetVertInterval()
{
	return vInterval;

		std::list<int>::iterator slidersIter;
	slidersIter = m_sliders.begin();
}