
#ifndef __SLIDER_H__
#define __SLIDER_H__


#include "Gui.h"
#include "Widget.h"

class Slider : public Widget
{
public:

	static void Slider::LoadTextures(const char *sliderTex, const char *buttonTex);

	Slider(Gui *gui, float width, float minValue, float maxValue, float initValue, const char *string);
	~Slider() {};

	float GetVal() { return m_value; };
	float SetVal(float value) { m_value = value; };

	void MouseButtonDown(int mouseButton);
	void MouseButtonUp(int mouseButton);

	void Render();

private:

	int m_sliderWidth;
	//int m_sliderHeight;
	int m_sliderX;
	int m_sliderY;

	int m_buttonY;

	float m_minValue;
	float m_maxValue;
	float m_value;

	int m_buttonWidth;
	int m_buttonHeight;
	bool m_buttonPressed;

	char *m_string;

	static int m_sliderHeight;
	static GLuint m_buttonTex; 
	static GLuint m_sliderTex; 
};

#endif /*__SLIDER_H__*/