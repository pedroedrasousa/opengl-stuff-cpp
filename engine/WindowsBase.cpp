
#include <windows.h>
#include <gl\gl.h>

#include "Main.h"
#include "OpenGLApp.h"

extern BaseApp *oglApp;

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	case WM_KEYDOWN:
		if(wParam == VK_F4)
		{	
			oglApp->ShutdownAPI();
			oglApp->m_fullscreen = !oglApp->m_fullscreen;
			oglApp->InitAPI(WndProcedure);
			oglApp->Init();
		}
		break;
	case WM_SIZE:
		oglApp->m_width		= (int)LOWORD(lParam);
		oglApp->m_height	= (int)HIWORD(lParam);
		glViewport(0, 0, oglApp->m_width, oglApp->m_height);
		break;

	case WM_MOVE:
		oglApp->RefreshWindowPos( (int)LOWORD(lParam), (int)HIWORD(lParam) );
		break;;

	case WM_CLOSE:
		PostQuitMessage(WM_QUIT);
		break;


		case WM_LBUTTONDOWN:
			oglApp->m_mouseButton |= OpenGLApp::MBUTTON_LEFT;
			oglApp->MouseClick(OpenGLApp::MBUTTON_LEFT);
			break;
		case WM_LBUTTONUP:
			oglApp->m_mouseButton &= ~OpenGLApp::MBUTTON_LEFT;
			oglApp->MouseUp(OpenGLApp::MBUTTON_LEFT);
			break;
		case WM_MBUTTONDOWN:
			oglApp->m_mouseButton |= OpenGLApp::MBUTTON_MIDDLE;
			oglApp->MouseClick(OpenGLApp::MBUTTON_MIDDLE);
			break;
		case WM_MBUTTONUP:
			oglApp->m_mouseButton &= ~OpenGLApp::MBUTTON_MIDDLE;
			oglApp->MouseUp(OpenGLApp::MBUTTON_MIDDLE);
			break;
		case WM_RBUTTONDOWN:
			oglApp->m_mouseButton |= OpenGLApp::MBUTTON_RIGHT;
			oglApp->MouseClick(OpenGLApp::MBUTTON_RIGHT);
			break;
		case WM_RBUTTONUP:
			oglApp->m_mouseButton &= ~OpenGLApp::MBUTTON_RIGHT;
			oglApp->MouseUp(OpenGLApp::MBUTTON_RIGHT);
			break;


		/*
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
		*/

	//case WM_MOUSEMOVE:
		//oglApp->RefreshMousePos( (int)LOWORD(lParam), (int)HIWORD(lParam) );

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	oglApp = new BaseApp();

	oglApp->InitAPI(WndProcedure);
	oglApp->Init();

	for(;;)
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		oglApp->Run();
	}

	oglApp->ShutdownAPI();

	return msg.wParam;
}