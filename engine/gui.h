
#ifndef __GUI_H__
#define __GUI_H__

#include <windows.h>
#include <vector>

#include "gl\gl.h"
#include "Font.h"
#include "Image.h"
#include "Widget.h"
#include "Checkbox.h"
#include "Slider.h"

class Gui
{
public:

	Gui(Font *font);
	~Gui();

	int GetWidth() const{ return width; }
	int GetHeight() const{ return height; }

	Font* GetFont() const{ return font; }

	void Render(int mouseX, int mouseY, int mouseButton);

	void SetViewportDim(int width, int height);
	void AddWidget(Widget *widget);

	void MouseButtonDown(int mouseButton);
	void MouseButtonUp(int mouseButton);

	int GetNextWidgetPosX() const { return m_nextWidgetPosX; };
	int GetNextWidgetPosY() const { return m_nextWidgetPosY; };


	int m_mouseX;
	int m_mouseY;
	int m_oldMouseX;
	int m_oldMouseY;
	int m_mouseButtons;

private:

	int width;
	int height;

	int m_nextWidgetPosX;
	int m_nextWidgetPosY;

	Font *font;

	std::vector<Widget*> m_widgets;
};

#endif /*__GUI_H__*/