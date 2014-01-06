
#include <windows.h>
#include <list>

#include "Gui.h"
#include "OpenGLApp.h"
#include "RenderUtils.h"
#include "Texture.h"

Gui::Gui(Font *font)
{
	this->font = font;

	m_nextWidgetPosX = 10;
	m_nextWidgetPosY = 10;
}

Gui::~Gui()
{
	for(int i = 0; i < m_widgets.size(); i++)
	{
		delete m_widgets[i];
	}
}

void Gui::Render(int mouseX, int mouseY, int mouseButton)
{
	m_mouseX = mouseX;
	m_mouseY = mouseY;
	m_mouseButtons = mouseButton;

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	for(int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->Render();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	m_oldMouseX = mouseX;
	m_oldMouseY = mouseY;
}

void Gui::SetViewportDim(int width, int height)
{
	this->width = width;
	this->height = height;
}

void Gui::MouseButtonDown(int mouseButton)
{
	for(int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->MouseButtonDown(mouseButton);
	}

}

void Gui::MouseButtonUp(int mouseButton)
{
	for(int i = 0; i < m_widgets.size(); i++)
	{
		m_widgets[i]->MouseButtonUp(mouseButton);
	}
}

void Gui::AddWidget(Widget *widget)
{
	m_widgets.push_back(widget);

	m_nextWidgetPosY += 30;
}