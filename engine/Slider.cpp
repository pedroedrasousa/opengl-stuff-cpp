
#include <windows.h>

#include "Slider.h"
#include "gl\gl.h"
#include "Math.h"
#include "Texture.h"
#include "RenderUtils.h"
#include "OpenGLApp.h"

GLuint Slider::m_sliderTex;
GLuint Slider::m_buttonTex;
//int CheckBox::m_width;
int Slider::m_sliderHeight;

Slider::Slider(Gui *pGui, float width, float minValue, float maxValue, float initValue, const char *string) : Widget(pGui)
{
	m_sliderWidth = width;

	m_minValue = minValue;
	m_maxValue = maxValue;

	m_value = initValue;

	m_buttonWidth = 20;
	m_buttonHeight = 20;

	m_sliderHeight = 2;


	m_sliderX = pGui->GetNextWidgetPosX();
	m_sliderY = pGui->GetNextWidgetPosY();
	
	m_string = new char [ strlen(string) ];
	strcpy(m_string, string);
}

void Slider::Render()
{
	int y = m_sliderY - m_sliderHeight * 0.5f;

	glBindTexture(GL_TEXTURE_2D, m_sliderTex);

	RenderQuad(m_sliderX, m_sliderX + m_sliderWidth, y, y + m_sliderHeight);

	int x = m_sliderX + (m_value - m_minValue) / (m_maxValue - m_minValue) * m_sliderWidth  - m_buttonWidth * 0.5f;
	y = m_sliderY - m_buttonHeight * 0.5f;

	glBindTexture(GL_TEXTURE_2D, m_buttonTex);

	RenderQuad(x, x + m_buttonWidth, y, y + m_buttonHeight);


	m_pGui->GetFont()->Enable(m_pGui->GetWidth(), m_pGui->GetHeight());
	m_pGui->GetFont()->SetAlignment(FONT_HORIZ_ALIGN_LEFT, FONT_VERT_ALIGN_TOP);
	m_pGui->GetFont()->Print(m_sliderX + m_sliderWidth + 10, y, "%s: %f", m_string, m_value);	

	m_pGui->GetFont()->Disable();




		if(m_buttonPressed == true)
		{
			int newX = clamp(m_pGui->m_mouseX, m_sliderX, m_sliderX + m_sliderWidth);
			int oldX = clamp(m_pGui->m_oldMouseX, m_sliderX, m_sliderX + m_sliderWidth);

			int dx = newX - oldX;

			m_value += float(dx) / (float)m_sliderWidth * (m_maxValue - m_minValue);

			m_value = clamp(m_value, m_minValue, m_maxValue);

			callback.func(this, &m_value);
		}
}

void Slider::MouseButtonDown(int mouseButton)
{
	int y = m_sliderY - m_sliderHeight * 0.5f;
	int x = m_sliderX + (m_value - m_minValue) / (m_maxValue - m_minValue) * m_sliderWidth  - m_buttonWidth * 0.5f;

	if(mouseButton & OpenGLApp::MBUTTON_LEFT)
	{
		if(	m_pGui->m_mouseX > x && m_pGui->m_mouseX < x + m_buttonWidth &&
			m_pGui->m_mouseY > y - m_buttonHeight / 2 && m_pGui->m_mouseY < y + m_buttonHeight)
			m_buttonPressed = true;
	}
}

void Slider::MouseButtonUp(int mouseButton)
{
	if(mouseButton & OpenGLApp::MBUTTON_LEFT)
	{
		m_buttonPressed = false;
	}
}

void Slider::LoadTextures(const char *sliderTex, const char *buttonTex)
{
	Image image;

	if( !image.Load(sliderTex) )
	{
		char error[100];
		sprintf(error, "Unable to Load font:\n\'%s\'", sliderTex);
		ShowCursor(true);
		MessageBox(NULL, error, "Error", MB_OK | MB_ICONERROR);
		ShowCursor(false);
		return;
	}

	image.FlipVertical();

//	m_width = image.GetWidth();
	m_sliderHeight = image.GetHeight();

	glGenTextures(1, &m_sliderTex);
	glBindTexture(GL_TEXTURE_2D, m_sliderTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(image), image.GetWidth(), image.GetHeight(), 0, getPixelDataFormat(image), GL_UNSIGNED_BYTE, image.GetPixels());
	
	
	if( !image.Load(buttonTex) )
	{
		char error[100];
		sprintf(error, "Unable to Load font:\n\'%s\'", buttonTex);
		ShowCursor(true);
		MessageBox(NULL, error, "Error", MB_OK | MB_ICONERROR);
		ShowCursor(false);
		return;
	}

	image.FlipVertical();

	glGenTextures(1, &m_buttonTex);
	glBindTexture(GL_TEXTURE_2D, m_buttonTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(image), image.GetWidth(), image.GetHeight(), 0, getPixelDataFormat(image), GL_UNSIGNED_BYTE, image.GetPixels());
}
