
#include "Checkbox.h"

#include "OpenGLApp.h"
#include "Texture.h"
#include "RenderUtils.h"

GLuint CheckBox::m_boxTex;
GLuint CheckBox::m_markTex;
int CheckBox::m_width;
int CheckBox::m_height;

CheckBox::CheckBox(bool checked, const char *string, Gui *pGui) : Widget(pGui)
{
	m_pGui = pGui;

	m_checked = checked;

	m_xPos = pGui->GetNextWidgetPosX();;
	m_yPos = pGui->GetNextWidgetPosY();;

	m_string = new char [ strlen(string) ];
	strcpy(m_string, string);
}

void CheckBox::Render()
{
	glBindTexture(GL_TEXTURE_2D, m_boxTex);
	RenderQuad(m_xPos, m_xPos + m_width, m_yPos, m_yPos + m_height);

	if(m_checked)
	{
		glBindTexture(GL_TEXTURE_2D, m_markTex);
		RenderQuad(m_xPos, m_xPos + m_width, m_yPos, m_yPos + m_height);
	}

	m_pGui->GetFont()->Enable(m_pGui->GetWidth(), m_pGui->GetHeight());
	m_pGui->GetFont()->SetAlignment(FONT_HORIZ_ALIGN_LEFT, FONT_VERT_ALIGN_TOP);
	m_pGui->GetFont()->Print(m_xPos + m_width * 1.5f, m_yPos, "%s", m_string);	
	m_pGui->GetFont()->Disable();
}

void CheckBox::MouseButtonDown(int mouseButton)
{
	if(mouseButton & OpenGLApp::MBUTTON_LEFT)
	{
		if(m_pGui->m_mouseX > m_xPos && m_pGui->m_mouseX < m_xPos + m_width &&
		   m_pGui->m_mouseY > m_yPos && m_pGui->m_mouseY < m_yPos + m_height)
					m_checked = !m_checked;

		callback.func(this, &m_checked);
	}
}

void CheckBox::LoadTextures(const char *boxTex, const char *markTex)
{
	Image image;

	if( !image.Load(boxTex) )
	{
		char error[100];
		sprintf(error, "Unable to Load font:\n\'%s\'", boxTex);
		ShowCursor(true);
		MessageBox(NULL, error, "Error", MB_OK | MB_ICONERROR);
		ShowCursor(false);
		return;
	}

	image.FlipVertical();

	m_width = image.GetWidth();
	m_height = image.GetHeight();

	glGenTextures(1, &m_boxTex);
	glBindTexture(GL_TEXTURE_2D, m_boxTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(image), image.GetWidth(), image.GetHeight(), 0, getPixelDataFormat(image), GL_UNSIGNED_BYTE, image.GetPixels());
	
	
	if( !image.Load(markTex) )
	{
		char error[100];
		sprintf(error, "Unable to Load font:\n\'%s\'", boxTex);
		ShowCursor(true);
		MessageBox(NULL, error, "Error", MB_OK | MB_ICONERROR);
		ShowCursor(false);
		return;
	}

	image.FlipVertical();

	glGenTextures(1, &m_markTex);
	glBindTexture(GL_TEXTURE_2D, m_markTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, getInternalFormat(image), image.GetWidth(), image.GetHeight(), 0, getPixelDataFormat(image), GL_UNSIGNED_BYTE, image.GetPixels());
}
