
#pragma comment(lib, "opengl32.lib")

#include <windows.h>
#include <gl\gl.h>
#include <stdio.h>
#include <list>

#include "OpenGLAPP.h"

OpenGLApp::OpenGLApp()
{
	m_width			= 1024;
	m_height		= 768;
	m_bpp			= 32;
	m_fullscreen	= false;

	m_hWnd		= NULL;
	m_hDC		= NULL;
	m_hRC		= NULL;
	m_hInstance	= NULL;

	m_mouseButton = 0;
}

bool OpenGLApp::InitAPI(WNDPROC WndProcedure)
{
	WNDCLASS	wndclass;
	DWORD		dwExStyle, dwStyle;
	GLuint		pixelFormat;
	RECT		winRect = {0, 0, m_width, m_height};

	m_hInstance = GetModuleHandle(NULL);

	wndclass.lpszClassName	= "OpenGL";
	wndclass.lpfnWndProc	= WndProcedure;
	wndclass.style			= 0;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= m_hInstance;
	wndclass.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= NULL;
	wndclass.lpszMenuName	= NULL;

	if( !RegisterClass(&wndclass) ) return false;

	if(m_fullscreen)
	{
		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);

		for(int i = 0; ;i++) 
		{
			if( !EnumDisplaySettings(NULL, i, &devMode) )
				break;

			m_deviceModes.push_front(devMode);
		}

		std::list<DEVMODE>::iterator devModeIter;
		std::list<DEVMODE>::iterator devModeIterMaxDisplayFreq;

		devModeIterMaxDisplayFreq = m_deviceModes.begin();

		for( devModeIter = m_deviceModes.begin(); devModeIter != m_deviceModes.end(); devModeIter++ )
		{
			if( devModeIter->dmPelsWidth == m_width &&
				devModeIter->dmPelsHeight == m_height &&
				devModeIter->dmBitsPerPel == m_bpp )
			{
				if(devModeIter->dmDisplayFrequency > devModeIterMaxDisplayFreq->dmDisplayFrequency)
					devModeIterMaxDisplayFreq = devModeIter;
			}
		}

		if( ChangeDisplaySettings( &(*devModeIterMaxDisplayFreq), CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
			return false;

		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;	
	}

	AdjustWindowRectEx(&winRect, dwStyle, false, dwExStyle);

	m_hWnd = CreateWindowEx(dwExStyle, "OpenGL", "Engine", dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
							winRect.right - winRect.left, winRect.bottom - winRect.top, NULL, NULL, m_hInstance, NULL);

	if(!m_hWnd) return false;

	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,								// version number
		PFD_DRAW_TO_WINDOW |			// support window
		PFD_SUPPORT_OPENGL |			// support OpenGL
		PFD_DOUBLEBUFFER,				// double buffered
		PFD_TYPE_RGBA,					// RGBA type
		24,								// 24-bit color depth
		0, 0, 0, 0, 0, 0,				// color bits ignored
		0,								// no alpha buffer
		0,								// shift bit ignored
		0,								// no accumulation buffer
		0, 0, 0, 0,						// accum bits ignored
		32,								// 32-bit z-buffer
		0,								// no stencil buffer 
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main layer
		0,								// reserved
		0, 0, 0							// layer masks ignored
	};

	m_hDC = GetDC(m_hWnd);
 	pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, pixelFormat, &pfd);
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	UpdateWindow(m_hWnd);

	return true;
}

bool OpenGLApp::ShutdownAPI()
{
	if(m_fullscreen)
		ChangeDisplaySettings(NULL, 0);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	DestroyWindow(m_hWnd);
	ReleaseDC(m_hWnd, m_hDC);
	UnregisterClass("OpenGL", m_hInstance);

	return true;
}

void OpenGLApp::RefreshWindowPos(int x, int y)
{
	m_winPosX = x;
	m_winPosY = y;
}

void OpenGLApp::RefreshMousePos(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;
}

void OpenGLApp::CenterMouse()
{
	SetCursorPos(m_mouseLockedX + m_winPosX, m_mouseLockedY + m_winPosY);
}

