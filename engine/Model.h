
#ifndef __MODEL_H__
#define __MODEL_H__

#include <windows.h>
#include <gl\gl.h>

#include <vector>

#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
#include "Frustum.h"
#include "Texture.h"

	struct BoundingBox {
		Vec3 v[8];
	};

	struct BoundingSphere {
		Vec3 offset;
		float radius;
	};


struct t_Face
{	
	int vertIndex[3];
	int neighbIndex[3];
	Plane plane;
	bool facingLight;
	DWORD smoothGroup;
};

struct t_Material
{
	char	strName[255];			// Nome associado ao material

	char	strDifuseTexFile[255];
	char	strBumpTexFile[255];
	
	float	ambient[3];
	float	difuse[3];
	float	specular[3];
	float	shininess;

	float	bumpiness;
	
	Texture	difuseTex;
	Texture	bumpTex;
};

struct t_TangentSpace
{
	Vec3 tangent;	// s tangent
	Vec3 binormal;	// t tangent
	Vec3 normal;
};

class Mesh 
{
public:	
	
	Mesh::Mesh() {};
	Mesh::~Mesh() {};

	void renderTriangles();

	void capShadow(Vec3 *lightPosObjSpc, bool front, bool back);
	void capShadowShader(Vec3 *lightPosObjSpc, bool front, bool back);

	void doShadowPass(Vec3 *lightPosObjSpc);
	void doShadowPassShader(Vec3 *lightPosObjSpc);

//protected:


//public:
	
	//int d;
	int				m_NumVerts;			// The number of verts in the model
	int				m_NumFaces;			// The number of faces in the model
	int				m_NumTexUV;			// The number of texture coordinates
	int				m_MaterialID;			// The texture ID to use, which is the index into our texture array
	bool			m_bHasTexture;			// This is TRUE if there is a texture map for this object
	char			m_strName[255];			// The name of the object
	int				*m_pVertIndex;			// indicies for the verts that make up this triangle
	BoundingBox		m_BoundingBox;
	BoundingSphere	m_BoundingSphere;
	Vec3			*m_pVerts;			// The object's vertices
	Vec2			*m_pTexVerts;		// The texture's UV coordinates
	t_Face			*m_pFaces;				// The faces information of the object
	t_TangentSpace	*m_pTangentSpace;
	Vec3			*m_pTangentSpaceLight;
};

class Model 
{
public:
	Model();
	~Model();

	void renderTriangles();
	void render(Mat4 &mPose, Frustum *frustum = NULL);
	void renderDOT3(Mat4 &mPose, Vec3 &lightPos, bool textures, Frustum *frustum = NULL);
	void render2(Mat4 &mPose, Vec3 &lightPos, bool textures, Frustum *frustum = NULL);

	void renderTmp(Mat4 *mPose, Vec3 &lightPos, float difuseValue, Frustum *frustum = NULL);
	void stencilShadow(Mat4 *mOccluderPose, Vec3 *lightPos, Frustum *frustum = NULL, bool selfShadowing = false);
	void computeTangentSpace();
	void computeConectivity();
	void computePlanes();
//	void computeBoundingSphere();
	void computeBoundingBox();
	void loadTextures();
	void renderBoundingBoxDebugLines(Mat4 const &mPose);
	void renderBoundingSphereDebugLines(Mat4 const &mPose);
	void renderTangentSpaceDebugLines();

	int m_NumMeshes;
	int m_NumMaterials;
	std::vector<t_Material> m_pMaterial;
	std::vector<Mesh> m_pMesh;
};

#endif /*__MODEL_H__*/