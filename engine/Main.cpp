
//#pragma comment(lib, "glu32.lib")

#include <windows.h>
#include <gl\gl.h>
//#include <gl\glut.h>

#include "gl\glext.h"
#include "OpenGlExt.h"

#include "Main.h"
#include "Image.h"
#include "HeightMap.h"
#include "Quadtree.h"
#include "Texture.h"
#include "Shader.h"
#include "3DS.h"
#include "Model.h"
#include "GUICallbacks.h"
#include "RenderUtils.h"

BaseApp *oglApp;

HeightMap *g_heightMap;

Importer3DS importer3DS;
Model model;

GLuint g_frameBuffer;

GLuint g_depthRenderBuffer;

GLuint g_renderBuffer[2];

Texture g_texFP16;
Texture g_texFP162;

GLuint g_depthRenderBuffer2;
Texture g_texWaterReflect;

GLuint g_depthRenderBufferHDR;
Texture g_texHDR;

GLuint g_renderBufferBlurdHDR;

Texture g_texHBluredHDR;
Texture g_texVBluredHDR;


void BaseApp::InitGUI()
{
	m_pGui = new Gui(m_pFont);

	CheckBox::LoadTextures("data\\textures\\checkbox.bmp", "data\\textures\\checkbox_mark.bmp");

	CheckBox *checkBox;
	checkBox = new CheckBox(false, "Wireframe", m_pGui);
	checkBox->AddCallBack(twfCallback);
	m_pGui->AddWidget(checkBox);

	CheckBox *checkBox2;
	checkBox2 = new CheckBox(false, "Render Quadtree Bounding Boxes", m_pGui);
	checkBox2->AddCallBack(QuadtreeBoundingBoxesCallback);
	m_pGui->AddWidget(checkBox2);

	CheckBox *checkBox3;
	checkBox3 = new CheckBox(true, "Usar Vertex Buffer Objects", m_pGui);
	checkBox3->AddCallBack(UseVBOSCallback);
	m_pGui->AddWidget(checkBox3);

	CheckBox *checkBox4;
	checkBox4 = new CheckBox(true, "Usar Geomipmaps", m_pGui);
	checkBox4->AddCallBack(UseGeoMorphingCallback);
	m_pGui->AddWidget(checkBox4);

	CheckBox *checkBox5;
	checkBox5 = new CheckBox(false, "Freeze visibility culling", m_pGui);
	checkBox5->AddCallBack(FreezeVisibCallback);
	m_pGui->AddWidget(checkBox5);



	Slider::LoadTextures("data\\textures\\slider.bmp", "data\\textures\\slider_btn.bmp");

	Slider *slider;
	slider = new Slider(m_pGui, 200, 0.1f, 180.0f, 50.0f, "FOV");
	slider->AddCallBack(fovCallback);
	m_pGui->AddWidget(slider);
/*
	Slider *slider2;
	slider2 = new Slider(m_pGui, 200, 0.0f, 20.0f, 0.2f, "Wind speed");
	slider2->AddCallBack(WindSpeedCallback);
	m_pGui->AddWidget(slider2);

	Slider *slider3;
	slider3 = new Slider(m_pGui, 200, 0.0f, 100.0f, 1.0f, "Wind strength");
	slider3->AddCallBack(WindStrengthCallback);
	m_pGui->AddWidget(slider3);
*/
	Slider *slider4;
	slider4 = new Slider(m_pGui, 200, 0.0f, 0.1f, 0.005f, "Blur offset");
	slider4->AddCallBack(BlurOffsetCallback);
	m_pGui->AddWidget(slider4);
}

void BaseApp::InitSkyDome()
{
	Mat4 mBias = Mat4(	0.5f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.5f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.5f, 0.0f,
						0.5f, 0.5f, 0.5f, 1.0f	);
	Mat4 m_proj;
	Mat4 m_view;

	Vec3 eye = Vec3(0, 0 - 500.0f, 0);
	Vec3 center = Vec3(0, 0 - 500.0f + 1.0f, 0);
	Vec3 up = Vec3(1.0f, 0.0f, 0.0f);

	m_proj.BuildProjection(40.0f, 1.0f, 0.5f, 10000.0f);
	m_view.BuildViewMatrix(eye, center, up);

	Mat4 textureMatrix = mBias * m_proj * m_view;

	m_skydome.Init(1000.0f, 1000.0f, 30, 180, 60, 90, textureMatrix);
}


void BaseApp::Init()
{
	m_wireframe = false;
	lockMouse = true;
	m_freezeFrustum = false;

	blurOffset = 0.01;

	LoadOpenGLExt();

	//importer3DS.Import3DS(&model, "data\\models\\3ds__tree2.3ds");

	m_pFont = new Font("data\\textures\\fonts\\tahoma_10.tga");

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	
	m_pFont->Enable(m_width, m_height);
	m_pFont->SetAlignment(FONT_HORIZ_ALIGN_LEFT, FONT_VERT_ALIGN_TOP);
	m_pFont->Print(10, 10, "Loading...");
	m_pFont->Disable();
	SwapBuffers(m_hDC);

	ShowCursor(false);

	InitGUI();

	InitSkyDome();

	Vec2 *lixo, *lixo3;
	Vec3 *lixo2;

	g_heightMap = new HeightMap();
	g_heightMap->SetWidthRatio(15.0f);
	g_heightMap->SetHeightRatio(5.0f);
	g_heightMap->SetYOffset(-150.0f);
	g_heightMap->LoadFromFile("data\\heightmap.png");

	int numVerts = 0;
	numVerts = g_heightMap->BuildArray(&lixo2, &lixo, &lixo3);

	g_quadtree = new Quadtree();
	g_quadtree->Build(g_heightMap, g_heightMap->vertices, lixo, numVerts);


	m_shdWater			= new Shader("data\\shaders\\water");
	m_shdWaterSpecular	= new Shader("data\\shaders\\water_specular");
	m_shdBlur			= new Shader("data\\shaders\\blur");
	m_shdHDR			= new Shader("data\\shaders\\hdr");
	m_shdSky			= new Shader("data\\shaders\\sky");
	m_shdUnderwater		= new Shader("data\\shaders\\underwater");
	m_shdSimpleColor	= new Shader("data\\shaders\\simple_color");

	m_sunPos.Set(0.0f, 1.0f, 0.0f);
		m_sunPos.x = cosf(0.35f);
		m_sunPos.y = 1.0 - sinf(0.35f);

	m_camera.PositionCamera(	-527.03f,		13.39f,		-606.4f,
								-526.3f,		13.4f,		-605.75f, 
								0.0f,		1.0f,		0.0f	);


	glViewport(0, 0, m_width, m_height);
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glMatrixMode(GL_PROJECTION);
	projMat.BuildProjection(50.0f, (float)m_width / (float)m_height, 0.1f, 50000.0f);
	glLoadMatrixf(projMat);
	glMatrixMode(GL_MODELVIEW);


	glGenRenderbuffersEXT(1, &g_renderBuffer[0]);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_renderBuffer[0]);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width, m_height);

	glGenRenderbuffersEXT(1, &g_renderBuffer[1]);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_renderBuffer[1]);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width, m_height);



	glGenFramebuffersEXT(1, &g_frameBuffer);
	glGenRenderbuffersEXT(1, &g_depthRenderBuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthRenderBuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width, m_height);

	//glGenFramebuffersEXT(1, &g_frameBuffer2);//
	glGenRenderbuffersEXT(1, &g_depthRenderBuffer2);//
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthRenderBuffer2);//
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 512, 512);//

	//glGenFramebuffersEXT(1, &g_frameBufferHDR);//
	glGenRenderbuffersEXT(1, &g_depthRenderBufferHDR);//
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthRenderBufferHDR);//
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width / 1, m_height / 1);//

	glGenRenderbuffersEXT(1, &g_renderBufferBlurdHDR);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_renderBufferBlurdHDR);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_width / 4, m_height / 4);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	switch(status)
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			//MessageBox(NULL,"GL_FRAMEBUFFER_COMPLETE_EXT!","SUCCESS",MB_OK|MB_ICONEXCLAMATION);
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			MessageBox(NULL,"GL_FRAMEBUFFER_UNSUPPORTED_EXT!","ERROR",MB_OK|MB_ICONEXCLAMATION);
			exit(0);
			break;

		default:
			exit(0);
	}

	g_texFP16.CreateRenderTarget(m_width, m_height, 3, GL_RGB16F_ARB, true);
	g_texFP162.CreateRenderTarget(m_width, m_height, 3, GL_RGB16F_ARB, true);

	g_texHDR.CreateRenderTarget(m_width / 1, m_height / 1, 3, GL_RGB16F_ARB, true);
	g_texHBluredHDR.CreateRenderTarget(m_width / 4, m_height / 4, 3, GL_RGB16F_ARB, true);
	g_texVBluredHDR.CreateRenderTarget(m_width / 4, m_height / 4, 3, GL_RGB16F_ARB, true);

	g_texWaterReflect.CreateRenderTarget(512, 512, 3, GL_RGB16F_ARB, true);


	Image image;

	image.Load("data\\textures\\water_nmap.png");
	image.ToNormalMap(2);
	water.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);

	image.Load("data\\textures\\teste.bmp");
	m_skyGradient.Load1D(image, GL_CLAMP, GL_LINEAR, GL_LINEAR);

	image.Load("data\\textures\\sun.png");
	m_texSun.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);

	CubeMapFilePath cubeMapFilePath;
	strcpy(cubeMapFilePath.posX, "data\\textures\\underwater_nmap.png");
	strcpy(cubeMapFilePath.negX, "data\\textures\\underwater_nmap.png");
	strcpy(cubeMapFilePath.posY, "data\\textures\\underwater_nmap.png");
	strcpy(cubeMapFilePath.negY, "data\\textures\\underwater_nmap.png");
	strcpy(cubeMapFilePath.posZ, "data\\textures\\underwater_nmap.png");
	strcpy(cubeMapFilePath.negZ, "data\\textures\\underwater_nmap.png");

	m_texCubeUnderwaterNormMap.LoadCubeMap(cubeMapFilePath, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 0);


	m_texViewPort.CreateRenderTarget(m_width, m_height, 3, GL_RGB, true);

	m_mouseLockedX = m_width * 0.5;
	m_mouseLockedY = m_height * 0.5;
	CenterMouse();
}

void DrawTile(float x, float y, float z, float size)
{
	size *= 0.5;

	glNormal3f( 0.0f, 1.0f, 0.0f );
	glBegin(GL_TRIANGLE_STRIP);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
		glVertex3f(x + size, y, z - size);

		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
		glVertex3f(x - size, y, z - size);

		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
		glVertex3f(x + size, y, z + size);

		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
		glVertex3f(x - size, y, z + size);
	glEnd();
}

void RenderInfinitePlane(const int tileSize, int nTilesRow, const float y0, const float cam_x, const float cam_z)
{
	int x, z;
	int x0 = int(cam_x / tileSize);
	int z0 = int(cam_z / tileSize);
	x0 *= tileSize;
	z0 *= tileSize;
	nTilesRow = int(nTilesRow * 0.5);

	for(x = x0 - nTilesRow * tileSize; x < x0 + nTilesRow * tileSize; x += tileSize)
	{
		for(z = z0 - nTilesRow * tileSize; z < z0 + nTilesRow * tileSize; z += tileSize)
		{
			DrawTile(float(x), y0, float(z), float(tileSize));
		}
	}
}

void BaseApp::RenderWaterReflection()
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texWaterReflect.GetID(), 0 );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthRenderBuffer2 );

	glViewport( 0, 0, 512, 512 );

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projMat);
	glMatrixMode(GL_MODELVIEW);


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	m_camera.BuildViewMatrix(&modelViewMat);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelViewMat);

	glScalef(1.0f, -1.0f, 1.0f);
	glFrontFace(GL_CW);


	glPushMatrix();
	glTranslatef(m_camera.m_vEye.x, -m_camera.m_vEye.y - 500, m_camera.m_vEye.z);
	glDepthMask(GL_FALSE);
	m_shdSky->enable();
	m_shdSky->sendUniform3f("color1", 25.5f, 25.5f, 38.0f);
	m_shdSky->sendUniform3f("color2", 0.5f, 0.8f, 1.0f);
	m_skydome.Render();
	m_shdSky->disable();
	glDepthMask(GL_TRUE);
	glPopMatrix();


	g_quadtree->SetRenderFoliageFlag(false);
	g_quadtree->RenderStaticLOD(frustum, m_camera.m_vEye, 3);
	g_quadtree->SetRenderFoliageFlag(true);

	glViewport( 0, 0, m_width, m_height);

	glFrontFace(GL_CCW);
}

void BaseApp::RenderUnderwaterNormalMap()
{
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texWaterReflect.GetID(), 0 );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthRenderBuffer2 );

	glViewport( 0, 0, 512, 512 );

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projMat);
	glMatrixMode(GL_MODELVIEW);


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

glDepthMask(GL_FALSE);
glDisable(GL_DEPTH_TEST);

	m_camera.BuildViewMatrix(&modelViewMat);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelViewMat);

	glScalef(1.0f, -1.0f, 1.0f);
	glFrontFace(GL_CW);


	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_texCubeUnderwaterNormMap.GetID());

	static float a = 0.0f;
	a+=0.02f;
	glPushMatrix();
	glTranslatef(m_camera.m_vEye.x, -m_camera.m_vEye.y - 500, m_camera.m_vEye.z);
	glDepthMask(GL_FALSE);


	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	//glTranslatef(a, 0.0f, 0.0f);
	glRotatef(a, 0.0f, 5.0f, 5.0f);

	m_skydome.RenderCubeMap();

	glRotatef(-2*a, 0.5f, 0.5f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	m_skydome.RenderCubeMap();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);



	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);


	glDepthMask(GL_TRUE);
	glPopMatrix();

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);

	glViewport( 0, 0, m_width, m_height);

	glFrontFace(GL_CCW);
}

void BaseApp::RenderWater()
{
	water.Bind(2);
	g_texFP162.Bind(1);
	g_texWaterReflect.Bind(0);

	Mat4 mBias = Mat4(	0.5f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.5f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.5f, 0.0f,
						0.5f, 0.5f, 0.5f, 1.0f	);

	Mat4 mModelViewProjection = mBias * projMat * modelViewMat;

	m_shdWater->enable();

	m_shdWater->sendUniform1i("g_texReflection", 0);
	m_shdWater->sendUniform1i("g_texRefraction", 1);
	m_shdWater->sendUniform1i("g_texNormalMap", 2);

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	m_shdWater->sendUniform1f("g_waterTranslation", float(time.QuadPart) * 0.00000000008f);
	m_shdWater->sendUniform3f("g_vEyePos", m_camera.m_vEye.x, m_camera.m_vEye.y, m_camera.m_vEye.z);
	m_shdWater->sendUniformMatrix4fv("g_mModelViewProjection", 1, 0, mModelViewProjection.m);

	glDisable(GL_CULL_FACE);
	RenderInfinitePlane(5000, 10, 0.0f, m_camera.m_vEye.x, m_camera.m_vEye.z);
	glEnable(GL_CULL_FACE);

	m_shdWater->disable();
}

void BaseApp::RenderWaterSpecular()
{

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glDisable(GL_CULL_FACE);

	water.Activate(0);

	m_shdWaterSpecular->enable();
	m_shdWaterSpecular->sendUniform1i("g_texNormalMap", 0);
	m_shdWaterSpecular->sendUniform3f("g_vLightPosObjSpc", 100.0f, 100.0f, 0.0f);
	m_shdWaterSpecular->sendUniform3f("g_vEyePosObjSpc", m_camera.m_vEye.x, m_camera.m_vEye.y, m_camera.m_vEye.z);

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	m_shdWaterSpecular->sendUniform1f("g_waterTranslation", float(time.QuadPart) * 0.00000000008f);

	glDisable(GL_CULL_FACE);
	RenderInfinitePlane(5000, 10, 0.0f, m_camera.m_vEye.x, m_camera.m_vEye.z);
	glEnable(GL_CULL_FACE);

	m_shdWaterSpecular->disable();

	water.Deactivate();

	
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

}

void RenderScreenCoveringQuad()
{
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, 1, 1, 0, 0, 1);	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2i(0, 1);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2i(1, 1);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2i(0, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(1, 0);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void BaseApp::BlurHDR()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_frameBuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texHDR.GetID(), 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthRenderBufferHDR);

	glViewport(0, 0, m_width / 1, m_height / 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glBindTexture(GL_TEXTURE_2D, g_texFP16.GetID());
	m_shdHDR->enable();
	RenderScreenCoveringQuad();
	m_shdHDR->disable();

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_renderBufferBlurdHDR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texHBluredHDR.GetID(), 0);
			
	glViewport(0, 0, m_width / 4, m_height / 4);

	m_shdBlur->enable();

	g_texHDR.Activate(0);
	m_shdBlur->sendUniform2f("blur_offset",  blurOffset, 0.0f);
	RenderScreenCoveringQuad();
	g_texHDR.Deactivate();

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texVBluredHDR.GetID(), 0);

	g_texHBluredHDR.Activate(0);
	m_shdBlur->sendUniform2f("blur_offset", blurOffset/2.0f, 0.0f);
	RenderScreenCoveringQuad();
	g_texHBluredHDR.Deactivate();



	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texHBluredHDR.GetID(), 0);

	g_texVBluredHDR.Activate(0);
	m_shdBlur->sendUniform2f("blur_offset", 0.0f, blurOffset);
	RenderScreenCoveringQuad();
	g_texVBluredHDR.Deactivate();

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texVBluredHDR.GetID(), 0);
	
	g_texHBluredHDR.Activate(0);
	m_shdBlur->sendUniform2f("blur_offset", 0.0f, blurOffset/2.0f);
	RenderScreenCoveringQuad();
	g_texHBluredHDR.Deactivate();


	m_shdBlur->disable();


	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	glViewport(0, 0, m_width, m_height);
}

void RenderSprite(float size, Vec3 *eyePos, Mat4 matModelView)
{
	if(size == 0)
		return;

	size /= 2;

	glPushMatrix();
    matModelView.Translate(*eyePos);
	matModelView = matModelView.Inverse();
	glMultMatrixf(matModelView.m);

	glDisable(GL_CULL_FACE);

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f( -size,  size, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( size,   size, 0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f( -size, -size, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( size,  -size, 0);
	glEnd();

	glEnable(GL_CULL_FACE);

	glPopMatrix();
}


void BaseApp::RenderFrame()
{	
	m_fpsCounter.MarkFrameStart();

	POINT mousePos;
	GetCursorPos(&mousePos);

	m_mouseX = mousePos.x - m_winPosX;
	m_mouseY = mousePos.y - m_winPosY;

	if(lockMouse)
	{
		m_camera.SetViewByMouse( float(m_mouseLockedX - m_mouseX) * 0.2f , float(m_mouseLockedY - m_mouseY) * 0.2f);	
		CenterMouse();
	}

	float height = g_heightMap->GetInterpolatedHeight(m_camera.m_vEye.x, m_camera.m_vEye.z);

	//if( m_camera.m_vEye.y <= height + 2.0f )
	{
		float oldY = m_camera.m_vEye.y;
		m_camera.m_vEye.y = height + 12.0f;
		float newY = m_camera.m_vEye.y;
		m_camera.m_vCenter.y += newY - oldY;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_frameBuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texFP16.GetID(), 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_renderBuffer[0]);

	glClearColor(0.8f, 1.0f, 0.8f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	

	m_camera.BuildViewMatrix(&modelViewMat);
	glLoadMatrixf(modelViewMat);


	if(!m_freezeFrustum)
		frustum.ComputeFrustum(projMat, modelViewMat);

	glPushMatrix();
	glTranslatef(m_camera.m_vEye.x, m_camera.m_vEye.y, m_camera.m_vEye.z);
	glDepthMask(GL_FALSE);
	m_shdSky->enable();
	m_shdSky->sendUniform3f("color1", 2.0f, 2.0f, 2.0f);
	m_shdSky->sendUniform3f("color2", 0.5f, 0.7f, 2.0f);
	m_skydome.Render();
	m_shdSky->disable();
	//glDepthMask(GL_TRUE);
	glPopMatrix();




	Vec3 sunPos = m_sunPos * 500.0f;
	glPopMatrix();
	glPushMatrix();
	glTranslatef(m_camera.m_vEye.x + sunPos.x, m_camera.m_vEye.y + sunPos.y, m_camera.m_vEye.z + sunPos.z);
	m_texSun.Activate(0);
	m_shdSimpleColor->enable();
	m_shdSimpleColor->sendUniform3f("color", 4.0f, 4.0f, 2.0f);
	glEnable(GL_BLEND );
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	RenderSprite(2000, &m_camera.m_vEye, modelViewMat);
	glDisable(GL_BLEND );
	m_shdSimpleColor->disable();
	m_texSun.Deactivate();


glDepthMask(GL_TRUE);
	glPopMatrix();



	g_quadtree->Render(frustum, m_camera.m_vEye);
/*
	glAlphaFunc(GL_GREATER,0.5f);
	glEnable(GL_ALPHA_TEST);
	Mat4 tmp;

	tmp.Identity();
	tmp.Translate(Vec3(0.0f, g_heightMap->GetInterpolatedHeight(0.0f, 0.0f), 0.0f));
	model.render(tmp, &frustum);

	tmp.Identity();
	tmp.Translate(Vec3(50.0f, g_heightMap->GetInterpolatedHeight(50.0f, 0.0f), 0.0f));
	model.render(tmp, &frustum);

	tmp.Identity();
	tmp.Translate(Vec3(0.0f, g_heightMap->GetInterpolatedHeight(0.0f, 100.0f), 100.0f));
	model.render(tmp, &frustum);

	tmp.Identity();
	tmp.Translate(Vec3(-50.0f, g_heightMap->GetInterpolatedHeight(-50.0f, -200.0f), -200.0f));
	model.render(tmp, &frustum);

	glDisable(GL_ALPHA_TEST);

*/

	RenderWaterReflection();

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texFP16.GetID(), 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_renderBuffer[0]);


	g_texFP162.Bind(0);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,0, 0, m_width, m_height);



	RenderWater();
	RenderWaterSpecular();

	


		RenderUnderwaterNormalMap();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// No wireframe
	glPushAttrib(GL_POLYGON_BIT);				
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	

	if(m_camera.m_vEye.y < 0)
	{

		m_shdUnderwater->enable();
		m_shdUnderwater->sendUniform1i("g_texViewport", 0);
		m_shdUnderwater->sendUniform1i("g_texNormalMap", 1);
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		m_shdUnderwater->sendUniform1f("g_waterTranslation", float(time.QuadPart) * 0.000000008f);
		g_texWaterReflect.Bind(1);
	}

	g_texFP16.Activate(0);
	RenderScreenCoveringQuad();
	g_texFP16.Deactivate();
/*
	g_texWaterReflect.Activate(0);
	RenderScreenCoveringQuad();
	g_texWaterReflect.Deactivate();
*/
	if(m_camera.m_vEye.y < 0)
	{
		m_shdUnderwater->disable();
	}

	BlurHDR();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	g_texVBluredHDR.Activate(0);
	RenderScreenCoveringQuad();
	g_texVBluredHDR.Deactivate();
	glDisable(GL_BLEND);

	m_pGui->SetViewportDim(m_width, m_height);

	m_pGui->Render(m_mouseX, m_mouseY, this->m_mouseButton);

	m_pFont->Enable(m_width, m_height);
	m_pFont->SetAlignment(FONT_HORIZ_ALIGN_RIGHT, FONT_VERT_ALIGN_TOP);
	m_pFont->Print(m_width - 10, 10, "FPS: %d", (int)m_fpsCounter.GetFPS() );
	m_pFont->Print(m_width - 10, 30, "Camera eye: %f, %f, %f", m_camera.m_vEye.x, m_camera.m_vEye.y, m_camera.m_vEye.z);	
	m_pFont->Print(m_width - 10, 50, "Camera center: %f, %f, %f", m_camera.m_vCenter.x, m_camera.m_vCenter.y, m_camera.m_vCenter.z);	
	m_pFont->Print(m_width - 10, 70, "Triangles rendered: %d", g_quadtree->GetNumTrisLastRendered());	
	m_pFont->Print(m_width - 10, 90, "VBOs data size: %d bytes", g_quadtree->m_vboDataSize);
	//m_pFont->Print(m_width - 10, 110, "Mouse coords: (%d, %d)", m_mouseX, m_mouseY);	
	m_pFont->Disable();
/*
	RenderTexDebug(3, 3, 2, g_texVBluredHDR.GetID(), "Blured HDR");
	RenderTexDebug(2, 3, 2, g_texHDR.GetID(), "HDR");
	RenderTexDebug(1, 3, 2, g_texFP16.GetID(), "Water refraction");
	RenderTexDebug(0, 3, 2, g_texWaterReflect.GetID(), "Water reflection");
*/
	glPopAttrib();

	SwapBuffers(m_hDC);

	m_fpsCounter.MarkFrameEnd();
}

void BaseApp::CheckKeyboardInput()
{	
	if( GetKeyState(VK_ESCAPE) & 0x80)
	{			
		PostQuitMessage(WM_QUIT);
	}

	if( GetKeyState('W') & 0x80)
	{
		m_camera.MoveForward(1.5f);
	}

	if( GetKeyState('S') & 0x80)
	{
		m_camera.MoveForward(-1.5f);
	}

	if( GetKeyState('A') & 0x80)
	{
		m_camera.Strafe(-1.5f);
	}

	if( GetKeyState('D') & 0x80)
	{			
		m_camera.Strafe(1.5f);
	}

	static float a = 0.0f;

	if( GetKeyState(VK_LEFT) & 0x80)
	{			
		a += 0.01f;
		m_sunPos.x = cosf(a);
		m_sunPos.y = 1.0 - sinf(a);
	}

	if( GetKeyState(VK_RIGHT) & 0x80)
	{			
		a -= 0.01f;
		m_sunPos.x = cosf(a);
		m_sunPos.y = 1.0 - sinf(a);
	}
}

bool BaseApp::Run()
{
	CheckKeyboardInput();
	RenderFrame();
	return true;
}

void BaseApp::MouseClick(int button)
{
	if(button & MBUTTON_MIDDLE)
	{
		lockMouse = !lockMouse;
		if(lockMouse)
			while (ShowCursor(FALSE) >= 0);
		else
			while (ShowCursor(TRUE) < 0);
	}

	if(button & MBUTTON_RIGHT)
	{
		ShowCursor(1);
		while (ShowCursor(TRUE) < 0);

	}

	m_pGui->MouseButtonDown(button);
}

void BaseApp::MouseUp(int button)
{
	m_pGui->MouseButtonUp(button);
}

void BaseApp::RenderTexDebug(int x, int y, int viewportFrac, GLuint texID, const char *string)
{
	int width = m_width >> viewportFrac;
	int height = m_height >> viewportFrac;

	int xpos = x * width;
	int ypos = y * height;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, m_width, m_height, 0, 0, 1);	
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(xpos, ypos + height);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(xpos + width, ypos + height);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(xpos, ypos);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(xpos + width, ypos);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	m_pFont->Enable(m_width, m_height);
	m_pFont->SetAlignment(FONT_HORIZ_ALIGN_LEFT, FONT_VERT_ALIGN_TOP);
	m_pFont->Print(xpos + 5, ypos + 5, string);	
	m_pFont->Disable();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}