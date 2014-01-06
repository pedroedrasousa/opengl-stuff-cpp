
#ifndef __GUICALLBACKS_H__
#define __GUICALLBACKS_H__

extern BaseApp *oglApp;

static void twfCallback(Widget *widget, void *data)
{
	bool *b = (bool *)(data);
	if(*b == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	oglApp->SetWireframeFlag(*b);
}

static void fovCallback(Widget *widget, void *data)
{
	float *b = (float *)(data);

	oglApp->GetProjMat().BuildProjection(*b, (float)oglApp->GetViewportWidth() / (float)oglApp->GetViewportHeight(), 0.1f, 50000.0f);
	Mat4 m = oglApp->GetProjMat();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(oglApp->GetProjMat());
	glMatrixMode(GL_MODELVIEW);
}

static void WindSpeedCallback(Widget *widget, void *data)
{
	float *b = (float *)(data);

	oglApp->GetQuadtree()->SetWindSpeed(*b);
}

static void BlurOffsetCallback(Widget *widget, void *data)
{
	float *b = (float *)(data);

	oglApp->blurOffset = *b;
}

static void WindStrengthCallback(Widget *widget, void *data)
{
	float *b = (float *)(data);

	oglApp->GetQuadtree()->SetWindStrength(*b);
}

static void QuadtreeBoundingBoxesCallback(Widget *widget, void *data)
{
	bool *b = (bool *)(data);

	oglApp->GetQuadtree()->SetRenderBoundingBoxesFlag(*b);
}

static void UseVBOSCallback(Widget *widget, void *data)
{
	bool *b = (bool *)(data);

	oglApp->GetQuadtree()->SetUseVBOSFlag(*b);
}

static void UseGeoMorphingCallback(Widget *widget, void *data)
{
	bool *b = (bool *)(data);

	oglApp->GetQuadtree()->SetUseGeomorphingFlag(*b);
}

static void FreezeVisibCallback(Widget *widget, void *data)
{
	bool *b = (bool *)(data);

	oglApp->SetFreezeFrustrumCalcFlag(*b);
}

#endif /*__GUICALLBACKS_H__*/