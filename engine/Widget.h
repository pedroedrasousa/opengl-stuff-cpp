
#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <windows.h>

#include "gl\gl.h"
#include "Image.h"
//#include "Gui.h"

class Gui;

class Widget {
public:
	
	Widget(Gui *pGui) { m_pGui = pGui; };
	~Widget() {};

	virtual void Render() = NULL;
	virtual void MouseButtonDown(int mouseButton) {}
	virtual void MouseButtonUp(int mouseButton) {}
	
	void AddCallBack(void (*func)(Widget *widget, void*), void *data = NULL);
	
protected:
	
	
	Gui *m_pGui;
	
	int m_posX;
	int m_posY;

	
	struct CallBack {
		void (*func)(Widget*, void*);
		void *data;
	};
	
	CallBack callback;
};

#endif /*__WIDGET_H__*/