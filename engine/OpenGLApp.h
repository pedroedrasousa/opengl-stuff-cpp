
#ifndef __OPENGLAPP_H__
#define __OPENGLAPP_H__

#include <windows.h>
#include <list>

#include "Frustum.h"

class OpenGLApp {

public:

	enum {
		MBUTTON_LEFT = 1 << 0,
		MBUTTON_MIDDLE = 1 << 1,
		MBUTTON_RIGHT = 1 << 2,
		MBUTTON_UP = 1 << 3,
		MBUTTON_DOWN = 1 << 4
	};

	OpenGLApp();
	bool InitAPI(WNDPROC WndProcedure);
	bool ShutdownAPI();

	void RefreshMousePos(int x, int y);
	void RefreshWindowPos(int x, int y);

	virtual void RenderFrame() = NULL;
	virtual bool Run() = NULL;

	virtual void MouseClick(int) {}
	virtual void MouseDblClick(int) {}

	int GetMouseX() { return m_mouseX; };
	int GetMouseY() { return m_mouseY; };

	int GetViewportWidth()	{ return m_width;	};
	int GetViewportHeight()	{ return m_height;	};



	int		m_winPosX;
	int		m_winPosY;
	int		m_width;
	int		m_height;
	int		m_bpp;
	bool	m_fullscreen;
	
	float	m_frameTime;

	std::list<DEVMODE> m_deviceModes;

	HWND		m_hWnd;
	HDC			m_hDC;
	HGLRC		m_hRC;
	HINSTANCE	m_hInstance;

	int m_mouseButton;

protected:
	int m_mouseX;
	int m_mouseY;

	//int m_oldMouseX;
	//int m_oldMouseY;

	int m_mouseLockedX;
	int m_mouseLockedY;

	Frustum frustum;



	void CenterMouse();
};

#endif /*__OPENGLAPP_H__*/