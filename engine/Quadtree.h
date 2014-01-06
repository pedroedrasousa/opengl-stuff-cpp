
#ifndef __Node_H__
#define __Node_H__

#include <vector>

#include "Vector.h"
#include "Frustum.h"
#include "HeightMap.h"
#include "Texture.h"
#include "Shader.h"
#include "Model.h"
#include "Matrix.h"

#define MAX_LODS 5
#define NUM_FOLIAGE_ARRAYS 2

class Quadtree;
class Node;


class Node {
private:

	typedef enum
	{
		TOP_LEFT_FRONT,
		TOP_LEFT_BACK,
		TOP_RIGHT_BACK,
		TOP_RIGHT_FRONT,
	} node_t;

	typedef enum
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	} neighbPatch_t;

	typedef struct
	{
		Vec3	pos;
		float	height;
		float	offset;
		Texture *texture;
	} foliage_t;

	typedef struct
	{
		Model *pModel;
		Mat4 *matPose;	
	} model_t;


public:
	Node(Quadtree *pQuadtree);
	~Node();

	int GetNumVerts(int *pIndices, int numIndices) const;
	void Build(Vec3 *pVerts, Vec2 *pTexCoords, int numVerts, Quadtree *quadtree);
	void FindEndNodes(Node **pEndNodes, Node *pNode) const;
	void FindNeighbEndNodes(Node **pEndNodes, Node *pNode, int numEndNodes);

	void AdjustToFitHeight(const Vec3 *pVerts, int numVerts);
	Vec3 GetNewNodeCenter(const Vec3 *pVerts, const Vec3 &m_vCenter, float m_width, node_t nodeID) const;
	node_t BelongingNode(Vec3 &vertex) const;
	void CreateNewNode(Vec3 *pVerts, Vec2 *pTexCoords, int numVerts, std::vector<bool> pVertList, Vec3 &m_vCenter, float m_width, node_t nodeID, int subDivision);
	void CreateNode(Vec3 *pVerts, Vec2 *pTexCoords, int numVerts);
	void AssignVerticesToNode(Vec3 *pVerts, Vec2 *pTexCoords, int numVerts);
	void PlaceFoliage(int foliageIndex, HeightMap &density);

	void BuildIndexArray(int LOD);
	void BuildIntermediateArray(int LOD);
	
	int CalcLOD(const Frustum &frustum);
	float CalcFactor(const Vec3 &eyePos);
	float DistToEye(const Vec3 &eyePos);

	void Render(Node *pNode, Frustum frustum, Vec3 &eyePos);
	void RenderFoliage(Node *pNode, Frustum frustum, Vec3 &eyePos, int foliageIndex);
	void RenderStaticLod(Node *pNode, Frustum frustum, Vec3 &eyePos, int LOD);

	Vec3 GetCenter() const		{	return m_vCenter;	}
	float GetWidth() const		{	return m_width;	}
	bool IsSubdivided() const	{   return subdivided;	}


	void Init();
	void Destroy();

private:

	std::vector<model_t> m_models;

	Quadtree *m_pQuadtree;

	Node	*pNode[4];
	bool		subdivided;
	int			m_subdivision;

	float	m_width;
	float	m_height;
	float	m_radius;
	Vec3	m_vCenter;

	Node	*m_pNeighbNode[4];


	int		m_LOD;


	float	*m_pVertsYOffset[MAX_LODS];	// Offset between one LOD and the coarser one, used to do geomorphing
	
	
	int		m_numVerts;
	Vec3	*m_pVerts;
    Vec2	*pTexCoords;


	GLuint m_vboVerticesID;
	GLuint m_vboTexCoordsID;

	GLuint m_vboFoliageVerticesID[NUM_FOLIAGE_ARRAYS];
	GLuint m_vboFoliageTexCoordsID[NUM_FOLIAGE_ARRAYS];
	GLuint m_vboFoliageIsTopVertexID[NUM_FOLIAGE_ARRAYS];
	GLuint m_vboFoliageStartOffsetsID[NUM_FOLIAGE_ARRAYS];

	Vec3	*m_foliageVertices[NUM_FOLIAGE_ARRAYS];
	Vec2	*m_foliageTexCoords[NUM_FOLIAGE_ARRAYS];
	short	*m_foliageIsTopVertex[NUM_FOLIAGE_ARRAYS];
	float	*m_foliageOfsets[NUM_FOLIAGE_ARRAYS];
	int		m_foliageNumVerts[NUM_FOLIAGE_ARRAYS];
};



class Quadtree {
public:

	Quadtree();
	~Quadtree() {};

	void Render(Frustum frustum, Vec3 &eyePos);
	void RenderStaticLOD(Frustum frustum, Vec3 &eyePos, int lod);

	void Build(HeightMap *pHeightMap, Vec3 *pVerts, Vec2 *pTexCoords, int numVerts);

	HeightMap *GetPHeightmap() const { return m_pHeightmap; };

	void SetRenderBoundingBoxesFlag(bool renderBoundingBoxes) { m_renderBoundingBoxes = renderBoundingBoxes; };
	bool GetRenderBoundingBoxesFlag() const { return m_renderBoundingBoxes; };
	void SetUseVBOSFlag(bool useVBOS) { m_useVBOS = useVBOS; };
	bool GetUseVBOSFlag() const { return m_useVBOS; };
	void SetUseGeomorphingFlag(bool useGeomorphing) { m_useGeomipmaps = useGeomorphing; };
	bool GetUseGeomorphingFlag() const { return m_useGeomipmaps; };

	void SetRenderFoliageFlag(bool renderFoliage) { m_renderFoliage = renderFoliage; };
	bool GetRenderFoliageFlag() const { return m_renderFoliage; };

	Texture &GetGrassTex() { return m_grassTex; };
	Texture &GetNormalTex() { return m_normalTex; };
	Texture &GetFoliageTex1() { return m_foliageTex1; };
	Texture &GetFoliageTex2() { return m_foliageTex2; };

	void IncNumTrisRendered(int numTrianglesRendered);
	int GetNumTrisLastRendered() const { return m_numTrisLastRendered; };


	Shader *GetTerrainShader() const { return m_pTerrainShader; };
	Shader *GetFoliageShader() const { return m_pFoliageShader; };

	float GetWindSpeed() const { return m_windSpeed; };
	void SetWindSpeed(float windSpeed) { m_windSpeed = windSpeed; };

	float GetWindStrength() const { return m_windStrength; };
	void SetWindStrength(float windStrength) { m_windStrength = windStrength; };

	void AddModel(Model &model);
	const std::vector<Model> GetModelList() const;



	int m_numIndices[MAX_LODS];
	int *m_pIndices[16][MAX_LODS];

	GLuint m_vboIndicesID[16][MAX_LODS];

	int m_vboDataSize;


	float	*m_pLeftVertex;
	float	*m_pRightVertex;
	float	*m_pTopVertex;
	float	*m_pBottomVertex;

	int		m_numTriangles[MAX_LODS];

	Texture m_sandTex;
	Texture m_sandAlpha;

	Model *m_models;


private:

	//std::vector<Model> m_models;

	int m_numTrisLastRendered;

	bool m_renderBoundingBoxes;
	bool m_useVBOS;
	bool m_useGeomipmaps;
	bool m_renderFoliage;

	HeightMap	*m_pHeightmap;
	Node		*m_pNode;

	Shader *m_pTerrainShader;
	Shader *m_pFoliageShader;

	Texture m_foliageTex1;
	Texture m_foliageTex2;


	Texture m_grassTex;
	Texture m_normalTex;

	float m_windSpeed;
	float m_windStrength;
};

#endif /*__Node_H__*/