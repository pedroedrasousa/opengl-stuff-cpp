
#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__

#include <windows.h>

#include "gl\gl.h"
#include "Font.h"
#include "Image.h"
#include "Gui.h"
#include "Widget.h"

#define CHECKBOX_STR_MAX_LENGHT 128

class CheckBox : public Widget
{
public:

	static void CheckBox::LoadTextures(const char *boxTex, const char *markTex);

	CheckBox(bool checked, const char *string, Gui *pGui);
	~CheckBox() {};

	float IsChecked() { return m_checked; };
	float SetChecked(bool checked) { m_checked = checked; };

	void Render();

	void MouseButtonDown(int mouseButton);

private:

	Gui *m_pGui;

	char *m_string;

	bool m_checked;

	int m_xPos;
	int m_yPos;


	static int m_width;
	static int m_height;

	static GLuint m_boxTex; 
	static GLuint m_markTex; 
};



#endif /*__CHECKBOX_H__*/