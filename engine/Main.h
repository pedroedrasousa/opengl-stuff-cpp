
#ifndef __BASEAPP_H__
#define __BASEAPP_H__

#include "OpenGLApp.h"
#include "Matrix.h"
#include "Camera.h"
#include "Gui.h"
#include "Font.h"
#include "FPSCounter.h"
#include "Quadtree.h"
#include "Shader.h"
#include "Skydome.h"

class BaseApp : public OpenGLApp {
public:

	void InitGUI();
	void InitSkyDome();
	void Init();

	void RenderFrame();
	void RenderWaterReflection();
	void RenderUnderwaterNormalMap();
	void RenderWaterFixedFunction();
	void RenderWater();
	void RenderWaterSpecular();
	void BlurHDR();

	bool Run();
	void CheckKeyboardInput();
	Mat4 &GetProjMat() { return projMat; }
	Quadtree *GetQuadtree() { return g_quadtree; }
	bool GetWireframeFlag() { return m_wireframe; }
	void SetWireframeFlag(bool wireframe) { m_wireframe = wireframe; }
	void SetFreezeFrustrumCalcFlag(bool freezeFrustum) { m_freezeFrustum = freezeFrustum; }

	void MouseClick(int button);
	void MouseUp(int button);
	void RenderTexDebug(int x, int y, int viewportFrac, GLuint texID, const char *string);

	float blurOffset;

private:

	Vec3 m_sunPos;

	Texture m_skyGradient;
	Texture m_texSun;
	Texture m_texCubeUnderwaterNormMap;

	bool m_wireframe;
	bool m_freezeFrustum;

	Skydome m_skydome;

	Texture m_texViewPort;
	Texture water;

	Shader *m_shdSky;
	Shader *m_shdWater;
	Shader *m_shdWaterSpecular;
	Shader *m_shdBlur;
	Shader *m_shdHDR;
	Shader *m_shdUnderwater;
	Shader *m_shdSimpleColor;

	FPSCounter m_fpsCounter;

	bool lockMouse;

	Gui *m_pGui;
	Font *m_pFont;
	Camera m_camera;

	Mat4 modelViewMat;
	Mat4 projMat;

	Quadtree *g_quadtree;
};

#endif /*__BASEAPP_H__*/