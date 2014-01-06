
#define _CRT_RAND_S
#include <stdlib.h>
#include <stdio.h>
#include <limits>

#include <windows.h>
#include <gl\gl.h>
//#include <gl\glut.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "Font.h"

#include "Quadtree.h"
#include "OpenGLExt.h"
#include "Camera.h"
#include "Renderer.h"
#include "HeightMap.h"
#include "Texture.h"
#include "Shader.h"
#include "Image.h"
#include "3DS.h"
#include "Model.h"

#define EPSILON 0

#define LOD1_DIST 100
#define LOD2_DIST 1000
#define LOD3_DIST 2000

int g_maxSubdivisions = 5;

extern Model model;

Importer3DS g_importer3DS;

Node::Node(Quadtree *pQuadtree)
{
	m_pNeighbNode[LEFT]		= NULL;
	m_pNeighbNode[RIGHT]	= NULL;
	m_pNeighbNode[TOP]		= NULL;
	m_pNeighbNode[BOTTOM]	= NULL;

	m_pQuadtree = pQuadtree;

	m_subdivision = 0;

	Init();	
}

Node::~Node()
{
	Destroy();
}

int Node::GetNumVerts(int *pIndices, int numIndices) const
{
	int highestIndex = 0;

	for(int i = 0; i < numIndices; i++)  
	{
		if(pIndices[i] > highestIndex)
			highestIndex = pIndices[i]; 
	}

	return highestIndex + 1;
}

void Node::FindEndNodes(Node **pEndNodes, Node *pNode) const
{
	static int index = 0;

	if(!pNode)
		return;

	if( pNode->IsSubdivided() )
	{
		FindEndNodes(pEndNodes, pNode->pNode[TOP_LEFT_FRONT]);
		FindEndNodes(pEndNodes, pNode->pNode[TOP_LEFT_BACK]);
		FindEndNodes(pEndNodes, pNode->pNode[TOP_RIGHT_BACK]);
		FindEndNodes(pEndNodes, pNode->pNode[TOP_RIGHT_FRONT]);
	}
	else
	{
		pEndNodes[index++] = pNode;
	}
}

void Node::FindNeighbEndNodes(Node **pEndNodes, Node *pNode, int numEndNodes)
{
	int index = 0;
	Vec2 auxCenter;

	if(!pNode)
		return;

	bool found = false;

	if( pNode->IsSubdivided() )
	{
		FindNeighbEndNodes(pEndNodes, pNode->pNode[TOP_LEFT_FRONT], numEndNodes);
		FindNeighbEndNodes(pEndNodes, pNode->pNode[TOP_LEFT_BACK], numEndNodes);
		FindNeighbEndNodes(pEndNodes, pNode->pNode[TOP_RIGHT_BACK], numEndNodes);
		FindNeighbEndNodes(pEndNodes, pNode->pNode[TOP_RIGHT_FRONT], numEndNodes);
	}
	else
	{
		// Left
		auxCenter = Vec2( pNode->m_vCenter.x - pNode->m_width, pNode->m_vCenter.z);

		for(int i = 0; i < numEndNodes; i++)
		{
			if( auxCenter.Compare( Vec2(pEndNodes[i]->m_vCenter.x, pEndNodes[i]->m_vCenter.z), EPSILON ) )
				pNode->m_pNeighbNode[0] = pEndNodes[i];
			else
				continue;

			break;
		}

		// Right
		auxCenter = Vec2( pNode->m_vCenter.x + pNode->m_width, pNode->m_vCenter.z);

		for(int i = 0; i < numEndNodes; i++)
		{
			if( auxCenter.Compare( Vec2(pEndNodes[i]->m_vCenter.x, pEndNodes[i]->m_vCenter.z), EPSILON ) )
				pNode->m_pNeighbNode[1] = pEndNodes[i];
			else
				continue;

			break;
		}

		// Top
		auxCenter = Vec2( pNode->m_vCenter.x, pNode->m_vCenter.z - pNode->m_width);

		for(int i = 0; i < numEndNodes; i++)
		{
			if( auxCenter.Compare( Vec2(pEndNodes[i]->m_vCenter.x, pEndNodes[i]->m_vCenter.z), EPSILON ) )
				pNode->m_pNeighbNode[2] = pEndNodes[i];
			else
				continue;

			break;
		}

		// Bottom
		auxCenter = Vec2( pNode->m_vCenter.x, pNode->m_vCenter.z + pNode->m_width);

		for(int i = 0; i < numEndNodes; i++)
		{
			if( auxCenter.Compare( Vec2(pEndNodes[i]->m_vCenter.x, pEndNodes[i]->m_vCenter.z), EPSILON ) )
				pNode->m_pNeighbNode[3] = pEndNodes[i];
			else
				continue;

			break;
		}
	}
}

void Node::AdjustToFitHeight(const Vec3 *pVerts, int numVerts)
{
	float maxHeight = 0;
	float minY = FLT_MAX, maxY = -FLT_MAX;

	if(!pVerts || numVerts <= 0)
		return;

	for(int i = 0; i < numVerts; i++)
	{
		if(pVerts[i].y > maxY)
			maxY = pVerts[i].y;
		else if(pVerts[i].y < minY)
			minY = pVerts[i].y;
	}

	m_vCenter.y = minY + (maxY - minY) / 2;	
	m_height = maxY - minY;

	for(int i = 0; i < numVerts; i++)	// Bounding sphere ?????
	{
		if( Length(m_vCenter - pVerts[i]) > m_radius)
			m_radius = Length(m_vCenter - pVerts[i]);
	}
}

void Node::CreateNewNode(Vec3 *m_pVerts, Vec2 *pTexCoords, int numVerts, std::vector<bool> pVertList, Vec3 &m_vCenter, float m_width, node_t nodeID, int subdivision)
{
	if(numVerts <= 0)
		return;

	int numNewNodeVerts = 0;

	for(int i = 0; i < numVerts; i++)  
	{
		if(pVertList[i])
			numNewNodeVerts++;
	}
	
	Vec3 *pNodeVerts = new Vec3[numNewNodeVerts];
	Vec2 *pNodeTexCoords = new Vec2[numNewNodeVerts];

	int index = 0;
	int nodeVertIndex = 0;

	for(int i = 0; i < numVerts; i++)
	{
		if(pVertList[i])
		{
			pNodeVerts[index] = m_pVerts[i];
			pNodeTexCoords[index] = pTexCoords[i];
			index++;
		}
	}
	
	pNode[nodeID] = new Node(m_pQuadtree);
	pNode[nodeID]->m_subdivision = subdivision;
	pNode[nodeID]->CreateNode(pNodeVerts, pNodeTexCoords, numNewNodeVerts);

	delete[] pNodeVerts;
	delete[] pNodeTexCoords;
}

void Node::CreateNode(Vec3 *pVerts, Vec2 *pTexCoords, int numVerts)
{
	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;
	float minZ = FLT_MAX, maxZ = -FLT_MAX;

	// Find the center and width

	for(int i = 0; i < numVerts; i++)
	{
		if(pVerts[i].x > maxX)
			maxX = pVerts[i].x;
		else if(pVerts[i].x < minX)
			minX = pVerts[i].x;

		if(pVerts[i].y > maxY)
			maxY = pVerts[i].y;
		else if(pVerts[i].y < minY)
			minY = pVerts[i].y;

		if(pVerts[i].z > maxZ)
			maxZ = pVerts[i].z;
		else if(pVerts[i].z < minZ)
			minZ = pVerts[i].z;
	}

	m_vCenter.x = minX + (maxX - minX) / 2;	
	m_vCenter.y = minY + (maxY - minY) / 2;	
	m_vCenter.z = minZ + (maxZ - minZ) / 2;	

	m_width = maxX - minX;



	int highestIndex = 0;

	int numTriangles = ( sqrtf(numVerts) - 1) * 2 * ( sqrtf(numVerts) - 1);

	AdjustToFitHeight(pVerts, numVerts);

	if(m_subdivision >= g_maxSubdivisions)
	{
		// Max subdivisions reached
		subdivided = false;
		AssignVerticesToNode(pVerts, pTexCoords, numVerts);
	}
	else
	{
		subdivided = true;

		// Create the list of booleans for each triangle index
		std::vector<bool> pVertList1(numVerts);		// TOP_LEFT_FRONT
		std::vector<bool> pVertList2(numVerts);		// TOP_LEFT_BACK
		std::vector<bool> pVertList3(numVerts);		// TOP_RIGHT_BACK
		std::vector<bool> pVertList4(numVerts);		// TOP_RIGHT_FRONT

		for(int i = 0; i < numVerts; i++)
		{
			Vec3 *pVertex = &pVerts[i];

			// Check if the point lines within the TOP LEFT FRONT node
			if( (pVertex->x <= m_vCenter.x + EPSILON) && (pVertex->z >= m_vCenter.z - EPSILON) )
				pVertList1[i] = true;

			// Check if the point lines within the TOP LEFT BACK node
			if( (pVertex->x <= m_vCenter.x + EPSILON) && (pVertex->z <= m_vCenter.z + EPSILON) )
				pVertList2[i] = true;

			// Check if the point lines within the TOP RIGHT BACK node
			if( (pVertex->x >= m_vCenter.x - EPSILON) && (pVertex->z <= m_vCenter.z + EPSILON) )
				pVertList3[i] = true;

			// Check if the point lines within the TOP RIGHT FRONT node
			if( (pVertex->x >= m_vCenter.x - EPSILON) && (pVertex->z >= m_vCenter.z - EPSILON) )
				pVertList4[i] = true;
		}	

		CreateNewNode(pVerts, pTexCoords, numVerts, pVertList1, m_vCenter, m_width, TOP_LEFT_FRONT, m_subdivision + 1);
		CreateNewNode(pVerts, pTexCoords, numVerts, pVertList2, m_vCenter, m_width, TOP_LEFT_BACK, m_subdivision + 1);
		CreateNewNode(pVerts, pTexCoords, numVerts, pVertList3, m_vCenter, m_width, TOP_RIGHT_BACK, m_subdivision + 1);
		CreateNewNode(pVerts, pTexCoords, numVerts, pVertList4, m_vCenter, m_width, TOP_RIGHT_FRONT, m_subdivision + 1);
	}
}

void Node::BuildIndexArray(int LOD)
{
	int index = 0;
	int iVert = 0;

    int m_width = sqrtf(m_numVerts);

    int currentWidth = m_width;
	for(int i = 0; i < LOD; i++)
		currentWidth = currentWidth / 2 + 1;

	int currentNumVerts = currentWidth * currentWidth;

	m_pQuadtree->m_numTriangles[LOD] = ( currentWidth - 1) * ( currentWidth - 1) * 2;

	int step = pow(2.0f, LOD);
	int step_minus_one = step - 1;

	for(int i = 0; i < 16; i++)
	{
		m_pQuadtree->m_numIndices[LOD] = (currentNumVerts - currentWidth) * 2 + (currentWidth - 2) * 2;
		m_pQuadtree->m_pIndices[i][LOD] = new int[ m_pQuadtree->m_numIndices[LOD] ];
	}

	bool direction = false;

	for (int i = 0; i < m_width - 1; i += step)
	{
		direction = !direction;

		if(direction == true && i != 0)
		{
			iVert += m_width * step_minus_one - step_minus_one;

			m_pQuadtree->m_pIndices[0][LOD][index++] = iVert;
			m_pQuadtree->m_pIndices[0][LOD][index++] = iVert;
		}
		else if(direction == false && i != 0)
		{
			iVert += m_width * step_minus_one - step_minus_one;

			m_pQuadtree->m_pIndices[0][LOD][index++] = (iVert / m_width) * m_width + m_width - (iVert % m_width) - 1;
			m_pQuadtree->m_pIndices[0][LOD][index++] = (iVert / m_width) * m_width + m_width - (iVert % m_width) - 1;
		}

		for(int j = 0; j < m_width; j += step)
		{
			if(direction == true)
			{
				m_pQuadtree->m_pIndices[0][LOD][index++] = iVert + m_width * step;
				m_pQuadtree->m_pIndices[0][LOD][index++] = iVert;

				iVert += step;
			}
			else
			{
				m_pQuadtree->m_pIndices[0][LOD][index++] = (iVert / m_width) * m_width + m_width - (iVert % m_width) - 1;
				m_pQuadtree->m_pIndices[0][LOD][index++] = (iVert / m_width) * m_width + m_width - (iVert % m_width) + m_width * step - 1;

				iVert += step;
			}

		}
	}

	if(LOD == 3)	// Lowest LOD - No need to adapt to fit neighbour patches
		return;

	for(int i = 1; i < 16; i++)
		memcpy(m_pQuadtree->m_pIndices[i][LOD], m_pQuadtree->m_pIndices[0][LOD], sizeof(int) * m_pQuadtree->m_numIndices[LOD]);

	// Adjust edges to fit LOD changes
	for(int patch = 1; patch < 16; patch++)
	{

		if(patch == 1 || patch == 3 || patch == 5 || patch == 7 || patch == 9 || patch == 11 || patch == 13 || patch == 15)
			// Left
			for(int i = 0; i < m_pQuadtree->m_numIndices[LOD]; i++)
			{
				if(m_pQuadtree->m_pIndices[patch][LOD][i] % (step * 2) > 0 && m_pQuadtree->m_pIndices[patch][LOD][i] < m_width)
					m_pQuadtree->m_pIndices[patch][LOD][i] += step;
			}
		if(patch == 2 || patch == 3 || patch == 6 || patch == 7 || patch == 10 || patch == 11 || patch == 14 || patch == 15)
			// Right
			for(int i = 0; i < m_pQuadtree->m_numIndices[LOD]; i++)
			{
				if(m_pQuadtree->m_pIndices[patch][LOD][i] % (step * 2) > 0 && m_pQuadtree->m_pIndices[patch][LOD][i] > m_width * m_width - m_width)
					m_pQuadtree->m_pIndices[patch][LOD][i] -= step;
			}

		if(patch == 4 || patch == 5 || patch == 6 || patch == 7 || patch == 12 || patch == 13 || patch == 14 || patch == 15)
			// Up
			for(int i = 0; i < m_pQuadtree->m_numIndices[LOD]; i++)
			{
				if(m_pQuadtree->m_pIndices[patch][LOD][i] % m_width == 0 && m_pQuadtree->m_pIndices[patch][LOD][i] % (2 * step * m_width) > 0)
					m_pQuadtree->m_pIndices[patch][LOD][i] += step * m_width;
			}

		if(patch == 8 || patch == 9 || patch == 10 || patch == 11 || patch == 12 || patch == 13 || patch == 14 || patch == 15)
			// Down
			for(int i = 0; i < m_pQuadtree->m_numIndices[LOD]; i++)
			{
				if( (m_pQuadtree->m_pIndices[patch][LOD][i] - m_width + 1) % m_width == 0 && (m_pQuadtree->m_pIndices[patch][LOD][i] - m_width + 1) % (2 * step * m_width) > 0 && m_pQuadtree->m_pIndices[patch][LOD][i] > m_width && m_pQuadtree->m_pIndices[patch][LOD][i] < m_width * m_width - 1)
					m_pQuadtree->m_pIndices[patch][LOD][i] -= step * m_width;
			}
	}
}


void Node::BuildIntermediateArray(int LOD)
{
    int m_width = sqrtf(m_numVerts);

    int currentWidth = m_width;
	for(int i = 0; i < LOD; i++)
		currentWidth = currentWidth / 2 + 1;

	int currentNumVerts = currentWidth * currentWidth;

	int step = pow(2.0f, LOD);

	// Create a vertex array to allow geomorphing between a LOD and the coarser one
	m_pVertsYOffset[LOD] = new float[m_numVerts];

	float newYPos;

	int index = -1;

	for(int i = 0; i < m_numVerts; i++)
	{
		if(i % m_width == 0 && i != 0) // Top vertex
			i += m_width * (step - 1);


		if(i % step != 0)
			continue; // This vertex doesn't correspond to this LOD

		index++;

		if(index % 2 == 0) // Even indices
		{
			if( (index / currentWidth) % 2 > 0 && (index % currentWidth) % 2 > 0 )									//	 __ __ __ __
			{																										//	|__.__|__.__|	
				newYPos = (m_pVerts[i - m_width * step - step].y + m_pVerts[i + m_width * step + step].y) * 0.5f;	//	|__|__|__|__|
				m_pVertsYOffset[LOD][i] = m_pVerts[i].y - newYPos;													//	|__.__|__.__|	
			}																										//	|__|__|__|__|
			else
			{
				// This vertex has the same position in both LODs
				m_pVertsYOffset[LOD][i] = 0.0f;
			}
		}
		else // Odd indices
		{
			if( index < currentWidth || index > currentNumVerts - currentWidth )	// Left and Right				//	 __ __ __ __
			{																										//	.__|__|__|__.
				newYPos = (m_pVerts[i - step].y + m_pVerts[i + step].y) * 0.5f;										//	|__|__|__|__|
				m_pVertsYOffset[LOD][i] = m_pVerts[i].y - newYPos;													//	.__|__|__|__.
			}																										//	|__|__|__|__|

			else if( index % currentWidth == 0 || (index + currentWidth * (step - 1) + 1) % currentWidth == 0 )	// Up and Down	//	 __.__ __.__
			{																																//	|__|__|__|__|
				newYPos = (m_pVerts[i - m_width * step].y + m_pVerts[i + m_width * step].y) * 0.5f;												//	|__|__|__|__|
				m_pVertsYOffset[LOD][i] = m_pVerts[i].y - newYPos;																				//	|__|__|__|__|
			
				//if( (index + currentWidth * (step - 1) + 1) % currentWidth == 0 )
					//i += m_width * (step - 1);
			}																																//	|__.__|__.__|

			else if( (index / currentWidth) % 2 > 0)																//	 __ __ __ __
			{																										//	|__|__|__|__|																								//	|__|__|__|__|
				newYPos = (m_pVerts[i - m_width * step].y + m_pVerts[i + m_width * step].y) * 0.5f;					//	|__.__|__.__|
				m_pVertsYOffset[LOD][i] = m_pVerts[i].y - newYPos;													//	|__|__|__|__|
			}																										//	|__|__|__|__|

			else if( (index / currentWidth) % 2 == 0)																//	 __ __ __ __
			{																										//	|__|__.__|__|
				newYPos = (m_pVerts[i - step].y + m_pVerts[i + step].y) * 0.5f;										//	|__|__|__|__|
				m_pVertsYOffset[LOD][i] = m_pVerts[i].y - newYPos;													//	|__|__.__|__|
			}																										//	|__|__|__|__|
			else
			{
				// This vertex has the same position in both LODs
				m_pVertsYOffset[LOD][i] = 0.0f;
			}
		}	
	}
}

void Node::PlaceFoliage(int foliageIndex, HeightMap &density)
{	
	int numVerticesAllocated = 8 * 50;

	m_foliageVertices[foliageIndex]		= (Vec3*)malloc(sizeof(Vec3) * 8 * 50);
	m_foliageTexCoords[foliageIndex]	= (Vec2*)malloc(sizeof(Vec2) * 8 * 50);
	m_foliageIsTopVertex[foliageIndex]	= (short*)malloc(sizeof(short) * 8 * 50);
	m_foliageOfsets[foliageIndex]		= (float*)malloc(sizeof(float) * 8 * 50);

	m_foliageNumVerts[foliageIndex] = 0;

	int vertIndex = 0;

	for(int i = -m_width / 2; i < m_width / 2; i += 4)
	{
		for(int j = -m_width / 2; j < m_width / 2; j += 4)
		{	
			unsigned int n;
			rand_s(&n);
			float f = (float)n / (float)UINT_MAX;
	
			float x = i;
			float z = j + f * 5.0f;

			float height = 1.5f + f * 10.0f * 1.5f;

			f = f * 10.0f / 4.3 * TWO_PI;


			float foliageDensity = density.GetInterpolatedHeight(m_vCenter.x + x, m_vCenter.z + z);

			unsigned int r;
			rand_s(&r);
			float b = (float)r / (float)UINT_MAX * 255.0f;


			if(b > 254.5f) {
				model_t mdl;
				mdl.matPose = new Mat4;
				mdl.matPose->Identity();
				mdl.matPose->Translate(Vec3(m_vCenter.x + x, m_pQuadtree->GetPHeightmap()->GetInterpolatedHeight(m_vCenter.x + x, m_vCenter.z + z), m_vCenter.z + z));
	//			mdl.pModel = *m_pQuadtree->GetModelList().begin();

				//std::vector<Model>::iterator modelIter;

				//for( modelIter = m_pQuadtree->GetModelList().begin(); modelIter != m_pQuadtree->GetModelList().end(); modelIter++ )
			
				m_models.push_back(mdl);
			}


			if(b > foliageDensity)
				continue;


			float y = m_pQuadtree->GetPHeightmap()->GetInterpolatedHeight(m_vCenter.x + x, m_vCenter.z + z);





			m_foliageNumVerts[foliageIndex] += 8;

			if(m_foliageNumVerts[foliageIndex] > numVerticesAllocated)
			{
				m_foliageVertices[foliageIndex]		= (Vec3*)realloc( m_foliageVertices[foliageIndex], sizeof(Vec3) * (numVerticesAllocated + 8 * 50) );
				m_foliageTexCoords[foliageIndex]	= (Vec2*)realloc( m_foliageTexCoords[foliageIndex], sizeof(Vec2) * (numVerticesAllocated + 8 * 50) );
				m_foliageIsTopVertex[foliageIndex]	= (short*)realloc( m_foliageIsTopVertex[foliageIndex], sizeof(short) * (numVerticesAllocated + 8 * 50) );
				m_foliageOfsets[foliageIndex]		= (float*)realloc( m_foliageOfsets[foliageIndex], sizeof(float) * (numVerticesAllocated + 8 * 50) );
				numVerticesAllocated += 8 * 50;
			}

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x - 3.0f;
			m_foliageVertices[foliageIndex][vertIndex].y = y;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(0.0f, 0.0f);
			vertIndex++;

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x + 3.0f;
			m_foliageVertices[foliageIndex][vertIndex].y = y;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(1.0f, 0.0f);
			vertIndex++;

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x + 3.0f;
			m_foliageVertices[foliageIndex][vertIndex].y = y + height;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(1.0f, 1.0f);
			m_foliageIsTopVertex[foliageIndex][vertIndex] = 1;
			m_foliageOfsets[foliageIndex][vertIndex] = f;
			vertIndex++;

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x - 3.0f;
			m_foliageVertices[foliageIndex][vertIndex].y = y + height;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(0.0f, 1.0f);
			m_foliageIsTopVertex[foliageIndex][vertIndex] = 1;
			m_foliageOfsets[foliageIndex][vertIndex] = f;
			vertIndex++;

			
			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x;
			m_foliageVertices[foliageIndex][vertIndex].y = y;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z - 3.0f;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(0.0f, 0.0f);
			vertIndex++;

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x;
			m_foliageVertices[foliageIndex][vertIndex].y = y;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z + 3.0f;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(1.0f, 0.0f);
			vertIndex++;

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x;
			m_foliageVertices[foliageIndex][vertIndex].y = y + height;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z + 3.0f;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(1.0f, 1.0f);
			m_foliageIsTopVertex[foliageIndex][vertIndex] = 1;
			m_foliageOfsets[foliageIndex][vertIndex] = f;
			vertIndex++;

			m_foliageVertices[foliageIndex][vertIndex].x = m_vCenter.x + x;
			m_foliageVertices[foliageIndex][vertIndex].y = y + height;
			m_foliageVertices[foliageIndex][vertIndex].z = m_vCenter.z + z - 3.0f;
			m_foliageTexCoords[foliageIndex][vertIndex] = Vec2(0.0f, 1.0f);
			m_foliageIsTopVertex[foliageIndex][vertIndex] = 1;
			m_foliageOfsets[foliageIndex][vertIndex] = f;
			vertIndex++;
		}
	}

	m_foliageVertices[foliageIndex]		= (Vec3*)realloc( m_foliageVertices[foliageIndex], m_foliageNumVerts[foliageIndex] * sizeof(Vec3) );
	m_foliageTexCoords[foliageIndex]	= (Vec2*)realloc( m_foliageTexCoords[foliageIndex], m_foliageNumVerts[foliageIndex] * sizeof(Vec2) );
	m_foliageIsTopVertex[foliageIndex]	= (short*)realloc( m_foliageIsTopVertex[foliageIndex], m_foliageNumVerts[foliageIndex] * sizeof(short) );
	m_foliageOfsets[foliageIndex]		= (float*)realloc( m_foliageOfsets[foliageIndex], m_foliageNumVerts[foliageIndex] * sizeof(float) );

	// Create VBOs
	if( m_pQuadtree->GetUseVBOSFlag() )
	{
		glGenBuffersARB(1, &m_vboFoliageVerticesID[foliageIndex]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboFoliageVerticesID[foliageIndex]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_foliageNumVerts[foliageIndex] * 3 * sizeof(float), m_foliageVertices[foliageIndex], GL_STATIC_DRAW_ARB);
		m_pQuadtree->m_vboDataSize += m_foliageNumVerts[foliageIndex] * 3 * sizeof(float);

		glGenBuffersARB(1, &m_vboFoliageTexCoordsID[foliageIndex]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboFoliageTexCoordsID[foliageIndex]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_foliageNumVerts[foliageIndex] * 2 * sizeof(float), m_foliageTexCoords[foliageIndex], GL_STATIC_DRAW_ARB);
		m_pQuadtree->m_vboDataSize += m_foliageNumVerts[foliageIndex] * 2 * sizeof(float);

		glGenBuffersARB(1, &m_vboFoliageIsTopVertexID[foliageIndex]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboFoliageIsTopVertexID[foliageIndex]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_foliageNumVerts[foliageIndex] * sizeof(short), m_foliageIsTopVertex[foliageIndex], GL_STATIC_DRAW_ARB);
		m_pQuadtree->m_vboDataSize += m_foliageNumVerts[foliageIndex] * sizeof(short);

		glGenBuffersARB(1, &m_vboFoliageStartOffsetsID[foliageIndex]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboFoliageStartOffsetsID[foliageIndex]);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_foliageNumVerts[foliageIndex] * sizeof(float), m_foliageOfsets[foliageIndex], GL_STATIC_DRAW_ARB);
		m_pQuadtree->m_vboDataSize += m_foliageNumVerts[foliageIndex] * sizeof(float);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}



void Node::AssignVerticesToNode(Vec3 *m_pVerts, Vec2 *pTexCoords, int numVerts)
{
	m_numVerts = numVerts;

	BuildIndexArray(0);
	BuildIndexArray(1);
	BuildIndexArray(2);
	BuildIndexArray(3);

	this->m_pVerts = new Vec3[numVerts];
	memset(this->m_pVerts, 0, sizeof(Vec3) * numVerts);
	memcpy(this->m_pVerts, m_pVerts, sizeof(Vec3) * numVerts);

	//m_pVertsYOffset[3] = new float[numVerts];
	//memset(m_pVerts, 0, sizeof(float) * numVerts); // ????


	BuildIntermediateArray(0);
	BuildIntermediateArray(1);
	BuildIntermediateArray(2);
	
	this->pTexCoords = new Vec2[numVerts];
	memset(this->pTexCoords, 0, sizeof(Vec2) * numVerts);
	memcpy(this->pTexCoords, pTexCoords, sizeof(Vec2) * numVerts);

	if( m_pQuadtree->GetUseVBOSFlag() )
	{
		// Create VBOs
		glGenBuffersARB(1, &m_vboVerticesID);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboVerticesID);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVerts * 3 * sizeof(float), m_pVerts, GL_STATIC_DRAW_ARB);
		m_pQuadtree->m_vboDataSize += numVerts * 3 * sizeof(float);

		glGenBuffersARB(1, &m_vboTexCoordsID);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vboTexCoordsID);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVerts * 2 * sizeof(float), pTexCoords, GL_STATIC_DRAW_ARB);
		m_pQuadtree->m_vboDataSize += numVerts * 2 * sizeof(float);
		
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

		static bool b = false;

		if(!b){
			b=true;
			
			for(int i = 0; i < 16; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					glGenBuffersARB(1, &m_pQuadtree->m_vboIndicesID[i][j]);
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_pQuadtree->m_vboIndicesID[i][j]);
					glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, m_pQuadtree->m_numIndices[j] * sizeof(int), m_pQuadtree->m_pIndices[i][j], GL_STATIC_DRAW_ARB);

					m_pQuadtree->m_vboDataSize += m_pQuadtree->m_numIndices[j] * sizeof(int);
				}
			}
		}

	} /*USE_VBOS*/

	Image image;
	image.Load("data\\grass.png");

	HeightMap heightMap;
	heightMap.SetWidthRatio(15.0f);
	heightMap.SetHeightRatio(1.0f);
	heightMap.SetYOffset(0.0f);
	heightMap.LoadFromFile("data\\grass.png");

	PlaceFoliage(0, heightMap);
	PlaceFoliage(1, heightMap);

}

void RenderCube(float px, float nx, float py, float ny, float pz, float nz)
{
	glBegin(GL_LINES);
		glVertex3f(px, py, pz);
		glVertex3f(px, py, nz);
			
		glVertex3f(px, py, nz);
		glVertex3f(nx, py, nz);
		
		glVertex3f(nx, py, nz);
		glVertex3f(nx, py, pz);

		glVertex3f(nx, py, pz);
		glVertex3f(px, py, pz);
	

		glVertex3f(px, ny, pz);
		glVertex3f(px, ny, nz);
			
		glVertex3f(px, ny, nz);
		glVertex3f(nx, ny, nz);
		
		glVertex3f(nx, ny, nz);
		glVertex3f(nx, ny, pz);

		glVertex3f(nx, ny, pz);
		glVertex3f(px, ny, pz);


		glVertex3f(px, py, pz);
		glVertex3f(px, ny, pz);
		
		glVertex3f(px, py, nz);
		glVertex3f(px, ny, nz);
		
		glVertex3f(nx, py, pz);
		glVertex3f(nx, ny, pz);

		glVertex3f(nx, py, nz);
		glVertex3f(nx, ny, nz);
	glEnd();
}

int Node::CalcLOD(const Frustum &frustum)
{
	float eyeDist = frustum.DistToEye(m_vCenter) - m_radius;

	if(eyeDist < LOD1_DIST)
		m_LOD = 0;
	else if(eyeDist < LOD2_DIST)	
		m_LOD = 1;
	else if(eyeDist < LOD3_DIST)	
		m_LOD = 2;
	else
		m_LOD = 3;

	return m_LOD;
}

float Node::CalcFactor(const Vec3 &eyePos)
{
	if(m_LOD == 0)
	{
		float factorSelf = (Length(eyePos - m_vCenter) - m_radius - LOD1_DIST) / (LOD1_DIST/1.5 - LOD1_DIST);
		if(factorSelf < 0.0f)
			factorSelf = 0.0f;
		else if(factorSelf > 1.0f)
			factorSelf = 1.0f;
		
		return factorSelf;
	}
	else if(m_LOD == 1)	
	{
		float factorSelf = (Length(eyePos - m_vCenter) - m_radius - LOD2_DIST) / (LOD2_DIST + LOD1_DIST/1.5 - LOD2_DIST);
		if(factorSelf < 0.0f)
			factorSelf = 0.0f;
		else if(factorSelf > 1.0f)
			factorSelf = 1.0f;

		return factorSelf;
	}
	else if(m_LOD == 2)	
	{
		float factorSelf = (Length(eyePos - m_vCenter) - m_radius - LOD3_DIST) / (LOD3_DIST + LOD1_DIST/1.5 - LOD3_DIST);
		if(factorSelf < 0.0f)
			factorSelf = 0.0f;
		else if(factorSelf > 1.0f)
			factorSelf = 1.0f;

		return factorSelf;
	}
	else
	{
		float factorSelf = (Length(eyePos - m_vCenter) - m_radius - LOD3_DIST) / (LOD3_DIST + LOD1_DIST/1.5 - LOD3_DIST);
		if(factorSelf < 0.0f)
			factorSelf = 0.0f;
		else if(factorSelf > 1.0f)
			factorSelf = 1.0f;

		return factorSelf;
	}
}

float Node::DistToEye(const Vec3 &eyePos)
{
	return Length(eyePos - m_vCenter) - m_radius;
}



void RenderQuad3d(float x, float y, float z, float height)
{
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x - 3.0f, y, z);

	glNormal3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x + 3.0f, y, z);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x - 3.0f, y + height, z);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x + 3.0f, y + height, z);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, y, z - 3.0f);

	glNormal3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x, y, z + 3.0f);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x, y + height, z - 3.0f);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x, y + height, z + 3.0f);
	glEnd();
}




void Node::Render(Node *pNode, Frustum frustum, Vec3 &eyePos)
{
	if(!pNode)
		return;

	if( pNode->IsSubdivided() && frustum.SphereInFrustum(pNode->m_vCenter, pNode->m_radius) && frustum.ParallelepipedInFrustum(pNode->m_vCenter, pNode->m_width, pNode->m_height, pNode->m_width) )
	{
		Render(pNode->pNode[TOP_LEFT_FRONT], frustum, eyePos);
		Render(pNode->pNode[TOP_LEFT_BACK], frustum, eyePos);
		Render(pNode->pNode[TOP_RIGHT_BACK], frustum, eyePos);
		Render(pNode->pNode[TOP_RIGHT_FRONT], frustum, eyePos);
	}
	else if( frustum.SphereInFrustum(pNode->m_vCenter, pNode->m_radius) && frustum.ParallelepipedInFrustum(pNode->m_vCenter, pNode->m_width, pNode->m_height, pNode->m_width) )
	{
		if( m_pQuadtree->GetRenderBoundingBoxesFlag() )
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPushMatrix();
			glTranslatef(pNode->m_vCenter.x, pNode->m_vCenter.y, pNode->m_vCenter.z); 
			RenderCube(pNode->m_width / 2, -pNode->m_width / 2, pNode->m_height / 2, -pNode->m_height / 2, pNode->m_width / 2, -pNode->m_width / 2);
			glPopMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		pNode->CalcLOD(frustum);

		if( m_pQuadtree->GetUseVBOSFlag() )
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboVerticesID);
			glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);
			glEnableClientState(GL_VERTEX_ARRAY);

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboTexCoordsID);
			glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			glVertexPointer(3, GL_FLOAT, 0, pNode->m_pVerts);
			glEnableClientState(GL_VERTEX_ARRAY);

			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, 0, pNode->pTexCoords);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glScalef(100.0f, 100.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		m_pQuadtree->GetGrassTex().Bind(0);
		m_pQuadtree->GetNormalTex().Bind(1);
		m_pQuadtree->m_sandTex.Bind(2);
		m_pQuadtree->m_sandAlpha.Bind(3);
		m_pQuadtree->GetGrassTex().Bind(0);

		Shader *terrainShader = m_pQuadtree->GetTerrainShader();
		terrainShader->enable();
		terrainShader->sendUniform1i("grassMap", 0);
		terrainShader->sendUniform1i("normalMap", 1);
		terrainShader->sendUniform1i("sandMap", 2);
		terrainShader->sendUniform1i("sandAlphaMap", 3);

		terrainShader->sendUniform3f("lightVec", 0.5f, 0.5f, 0.0f);

		if(pNode->m_LOD == 3)
		{
			// Lowest LOD - There's no need to adapt it to fit neighbour patches
		
			if( m_pQuadtree->GetUseVBOSFlag() )
			{
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_pQuadtree->m_vboIndicesID[0][3]);
				glDrawRangeElements(GL_TRIANGLE_STRIP, 0, m_pQuadtree->m_numIndices[3], m_pQuadtree->m_numIndices[3], GL_UNSIGNED_INT, NULL); 
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
			else
			{
				glDrawElements(GL_TRIANGLE_STRIP, m_pQuadtree->m_numIndices[3], GL_UNSIGNED_INT, m_pQuadtree->m_pIndices[0][3]);
			}
		
		}
		else
		{
			int patch		= 0;

			int leftLOD		= -1;
			int rightLOD	= -1;
			int topLOD		= -1;
			int bottomLOD	= -1;

			if(pNode->m_pNeighbNode[LEFT] != NULL)
			{
				leftLOD = pNode->m_pNeighbNode[LEFT]->CalcLOD(frustum);
			}

			if(pNode->m_pNeighbNode[RIGHT] != NULL)
			{
				rightLOD = pNode->m_pNeighbNode[RIGHT]->CalcLOD(frustum);
			}

			if(pNode->m_pNeighbNode[TOP] != NULL)
			{
				topLOD = pNode->m_pNeighbNode[TOP]->CalcLOD(frustum);
			}	

			if(pNode->m_pNeighbNode[BOTTOM] != NULL)
			{
				bottomLOD = pNode->m_pNeighbNode[BOTTOM]->CalcLOD(frustum);
			}

			// Find which patch to use based on the neighbours LODs

			int x;
			int y;

			if(leftLOD <= pNode->m_LOD && rightLOD <= pNode->m_LOD)
				x = 0;
			else if(leftLOD > pNode->m_LOD && rightLOD <= pNode->m_LOD)
				x = 1;
			else if(leftLOD <= pNode->m_LOD && rightLOD > pNode->m_LOD)
				x = 2;
			else if(leftLOD > pNode->m_LOD && rightLOD > pNode->m_LOD)
				x = 3;

			if(topLOD <= pNode->m_LOD && bottomLOD <= pNode->m_LOD)
				y = 0;
			else if(topLOD > pNode->m_LOD && bottomLOD <= pNode->m_LOD)
				y = 4;
			else if(topLOD <= pNode->m_LOD && bottomLOD > pNode->m_LOD)
				y = 8;
			else if(topLOD > pNode->m_LOD && bottomLOD > pNode->m_LOD)
				y = 12;

			patch = x + y;

			m_pQuadtree->IncNumTrisRendered( m_pQuadtree->m_numTriangles[pNode->m_LOD] );

			if( m_pQuadtree->GetUseVBOSFlag() )
			{
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_pQuadtree->m_vboIndicesID[patch][pNode->m_LOD]);
				glDrawRangeElements(GL_TRIANGLE_STRIP, 0, m_pQuadtree->m_numIndices[pNode->m_LOD], m_pQuadtree->m_numIndices[pNode->m_LOD], GL_UNSIGNED_INT, NULL); 
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
			else
			{
				glDrawElements(GL_TRIANGLE_STRIP, m_pQuadtree->m_numIndices[pNode->m_LOD], GL_UNSIGNED_INT, m_pQuadtree->m_pIndices[patch][pNode->m_LOD]);	
			}

		}

		if( m_pQuadtree->GetUseVBOSFlag() )
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

		terrainShader->disable();

		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		if(pNode->m_LOD < 2)
		{
			pNode->m_pQuadtree->GetFoliageTex1().Activate(0);
			pNode->RenderFoliage(pNode, frustum, eyePos, 0);
			pNode->m_pQuadtree->GetFoliageTex1().Deactivate();

			pNode->m_pQuadtree->GetFoliageTex2().Activate(0);
			pNode->RenderFoliage(pNode, frustum, eyePos, 1);
			pNode->m_pQuadtree->GetFoliageTex2().Deactivate();
		}

	}

	pNode->m_LOD = -1;
}

void Node::RenderFoliage(Node *pNode, Frustum frustum, Vec3 &eyePos, int foliageIndex)
{
	if(!m_pQuadtree->GetRenderFoliageFlag())
		return;

	if( m_pQuadtree->GetUseVBOSFlag() )
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboFoliageVerticesID[foliageIndex]);
		glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);
		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboFoliageTexCoordsID[foliageIndex]);
		glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboFoliageIsTopVertexID[foliageIndex]);
		glVertexAttribPointerARB(6, 1, GL_SHORT, GL_FALSE, 0, (char *) NULL);
		glEnableVertexAttribArrayARB(6);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboFoliageStartOffsetsID[foliageIndex]);
		glVertexAttribPointerARB(7, 1, GL_FLOAT, GL_FALSE, 0, (char *) NULL);
		glEnableVertexAttribArrayARB(7);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, 0, pNode->m_foliageVertices[foliageIndex]);
		glEnableClientState(GL_VERTEX_ARRAY);

		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(2, GL_FLOAT, 0, pNode->m_foliageTexCoords[foliageIndex]);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexAttribPointerARB(6, 1, GL_SHORT, GL_FALSE, 0, pNode->m_foliageIsTopVertex[foliageIndex]);
		glEnableVertexAttribArrayARB(6);

		glVertexAttribPointerARB(7, 1, GL_FLOAT, GL_FALSE, 0, pNode->m_foliageOfsets[foliageIndex]);
		glEnableVertexAttribArrayARB(7);
	}

	glDisable(GL_CULL_FACE);
	glAlphaFunc(GL_GREATER,0.3f);
	glEnable(GL_ALPHA_TEST);

	Shader *pShader = m_pQuadtree->GetFoliageShader();
	pShader->enable();
	pShader->sendUniform1i("texture", 0);
	pShader->sendUniform3f("eyePos", eyePos.x, eyePos.y, eyePos.z);

	pShader->sendUniform1f("fadeStartDist", 500.0f);
	pShader->sendUniform1f("fadeEndDist", 600.0f);
	pShader->sendUniform1f("windStrength", m_pQuadtree->GetWindStrength());


	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	pShader->sendUniform1f("offset", float(time.QuadPart) * 0.0000005f * m_pQuadtree->GetWindSpeed());


	glDrawArrays (GL_QUADS, 0, pNode->m_foliageNumVerts[foliageIndex]); 

	pShader->disable();

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);

	glDisableVertexAttribArrayARB(7);
	glDisableVertexAttribArrayARB(6);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if( m_pQuadtree->GetUseVBOSFlag() )
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);



		std::vector<model_t>::iterator modelIter;

		glAlphaFunc(GL_GREATER,0.5f);
		glEnable(GL_ALPHA_TEST);

		for( modelIter = pNode->m_models.begin(); modelIter != pNode->m_models.end(); modelIter++ )
		{
			m_pQuadtree->m_models->render(*modelIter->matPose, &frustum);
		}

		glDisable(GL_ALPHA_TEST);






	//m_pQuadtree->GetModelList()[0].render(frustum);
}

void Node::RenderStaticLod(Node *pNode, Frustum frustum, Vec3 &eyePos, int LOD)
{
	if(!pNode)
		return;

	if( pNode->IsSubdivided() && frustum.SphereInFrustum(pNode->m_vCenter, pNode->m_radius) && frustum.ParallelepipedInFrustum(pNode->m_vCenter, pNode->m_width, pNode->m_height, pNode->m_width) )
	{
		RenderStaticLod(pNode->pNode[TOP_LEFT_FRONT], frustum, eyePos, LOD);
		RenderStaticLod(pNode->pNode[TOP_LEFT_BACK], frustum, eyePos, LOD);
		RenderStaticLod(pNode->pNode[TOP_RIGHT_BACK], frustum, eyePos, LOD);
		RenderStaticLod(pNode->pNode[TOP_RIGHT_FRONT], frustum, eyePos, LOD);
	}
	else if( frustum.SphereInFrustum(pNode->m_vCenter, pNode->m_radius) && frustum.ParallelepipedInFrustum(pNode->m_vCenter, pNode->m_width, pNode->m_height, pNode->m_width) )
	{
		if( m_pQuadtree->GetRenderBoundingBoxesFlag() )
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPushMatrix();
			glTranslatef(pNode->m_vCenter.x, pNode->m_vCenter.y, pNode->m_vCenter.z); 
			RenderCube(pNode->m_width / 2, -pNode->m_width / 2, pNode->m_height / 2, -pNode->m_height / 2, pNode->m_width / 2, -pNode->m_width / 2);
			glPopMatrix();
			glColor3f(1.0f, 1.0f, 1.0f);
		}

		if( m_pQuadtree->GetUseVBOSFlag() )
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboVerticesID);
			glVertexPointer(3, GL_FLOAT, 0, (char *) NULL);
			glEnableClientState(GL_VERTEX_ARRAY);

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, pNode->m_vboTexCoordsID);
			glTexCoordPointer(2, GL_FLOAT, 0, (char *) NULL);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else
		{
			glVertexPointer(3, GL_FLOAT, 0, pNode->m_pVerts);
			glEnableClientState(GL_VERTEX_ARRAY);

			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glTexCoordPointer(2, GL_FLOAT, 0, pNode->pTexCoords);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glScalef(100.0f, 100.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		m_pQuadtree->GetGrassTex().Bind(0);
		m_pQuadtree->GetNormalTex().Bind(1);
		m_pQuadtree->m_sandTex.Bind(2);
		m_pQuadtree->m_sandAlpha.Bind(3);
		m_pQuadtree->GetGrassTex().Bind(0);

		Shader *terrainShader = m_pQuadtree->GetTerrainShader();
		terrainShader->enable();
		terrainShader->sendUniform1i("grassMap", 0);
		terrainShader->sendUniform1i("normalMap", 1);
		terrainShader->sendUniform1i("sandMap", 2);
		terrainShader->sendUniform1i("sandAlphaMap", 3);

		terrainShader->sendUniform3f("lightVec", 0.5f, 0.5f, 0.0f);

			// Lowest LOD - There's no need to adapt it to fit neighbour patches
		
			if( m_pQuadtree->GetUseVBOSFlag() )
			{
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_pQuadtree->m_vboIndicesID[0][LOD]);
				glDrawRangeElements(GL_TRIANGLE_STRIP, 0, m_pQuadtree->m_numIndices[LOD], m_pQuadtree->m_numIndices[LOD], GL_UNSIGNED_INT, NULL); 
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
			else
			{
				glDrawElements(GL_TRIANGLE_STRIP, m_pQuadtree->m_numIndices[LOD], GL_UNSIGNED_INT, m_pQuadtree->m_pIndices[0][LOD]);
			}
		

		if( m_pQuadtree->GetUseVBOSFlag() )
		{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

		terrainShader->disable();

		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		if(pNode->m_LOD < 2)
		{
			pNode->m_pQuadtree->GetFoliageTex1().Activate(0);
			pNode->RenderFoliage(pNode, frustum, eyePos, 0);
			pNode->m_pQuadtree->GetFoliageTex1().Deactivate();

			pNode->m_pQuadtree->GetFoliageTex2().Activate(0);
			pNode->RenderFoliage(pNode, frustum, eyePos, 1);
			pNode->m_pQuadtree->GetFoliageTex2().Deactivate();
		}

	}

	pNode->m_LOD = -1;

}

void Node::Init()
{
	subdivided = false;
	//m_subdivision = 0;
	m_LOD = -1;
	m_radius = 0;
	m_width = 0; 
	m_vCenter = Vec3(0.0f, 0.0f, 0.0f);
	m_pVerts = NULL;
	memset(pNode, 0, sizeof(pNode));
}

void Node::Destroy()
{
	if(m_pVerts)
	{
		delete[] m_pVerts;	
		m_pVerts = NULL;
	}

	for(int i = 0; i < 4; i++)
	{
		if(pNode[i])
		{
			delete pNode[i];
			pNode[i] = NULL;
		}
	}

	for(int i = 0; i < NUM_FOLIAGE_ARRAYS; i++)
	{
		delete [] m_foliageVertices[i];
		delete [] m_foliageTexCoords[i];
		delete [] m_foliageIsTopVertex[i];
		delete [] m_foliageOfsets[i];
	}


	glDeleteBuffersARB(1, &m_vboVerticesID);
	glDeleteBuffersARB(1, &m_vboTexCoordsID);

	Init();
}

//====================================================================================================
// Quadtree
//====================================================================================================

void CreateSandAlphaMap(Image &heightmap, Image &image)
{
	unsigned char *newPixels, *ca_src, *dst;

	const unsigned char *hm_src = heightmap.GetPixels();


	//ca_src = cliffAlphaMap.pixels;

	dst = newPixels = new unsigned char[heightmap.GetWidth() * heightmap.GetHeight()];

	for(int y = 0; y < heightmap.GetHeight(); y++)
	{
		for(int x = 0; x < heightmap.GetWidth(); x++)
		{
			//if(255 - *src > 358)
				//*dst = 255;
			//else

			//int tmp = *hm_src * 255 / 100;

			//if(*hm_src > 255)
				//tmp = 255;

			//tmp = 255 - tmp;

			//tmp *= 3;


			int tmp = clamp(70 - *hm_src, 0, 255);

			tmp *= 7;

			if(tmp < 0) *dst = 0;
			else if(tmp > 255) *dst = 255;
			else *dst = (BYTE)tmp;

			//*dst = 255;



			dst++;
			hm_src++;
			//ca_src++;
		}
	}

	image.SetWidth( heightmap.GetWidth() );
	image.SetHeight( heightmap.GetHeight() );
	image.SetNumChannels(1);
	image.SetPixels(newPixels);
	image.SetFormat(IFORMAT_I);
	//image.SnMipMaps	= 1;
}

Quadtree::Quadtree()
{
	Image image;
	Image sand;

	image.Load("Data\\textures\\grass2.bmp");
	m_grassTex.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);


	image.Load("Data\\heightmap.png");
	image.Resize(512, 512);
	image.ToNormalMap(5);
	m_normalTex.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);

	image.Load("Data\\heightmap.png");
	image.Resize(128, 128);
	CreateSandAlphaMap(image, sand);
	m_sandAlpha.Load2D(sand, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
	image.Load("Data\\textures\\dirt_simple_df_.dds");
	m_sandTex.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);


	image.Load("Data\\textures\\grass.bmp");
	m_foliageTex1.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);

	image.Load("Data\\textures\\foliage2.bmp");
	m_foliageTex2.Load2D(image, GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);

	m_pTerrainShader = new Shader("data\\shaders\\terrain");

	m_pFoliageShader = new Shader("data\\shaders\\foliage");
	m_pFoliageShader->bindAttribLocation("startingOffset", 7);
	m_pFoliageShader->bindAttribLocation("isTopVertex", 6);


	if( IsExtensionSupported("GL_ARB_vertex_buffer_object") )
		m_useVBOS = true;
	else
		m_useVBOS = false;

	m_renderFoliage = true;
	m_renderBoundingBoxes = false;
	m_vboDataSize = 0;
	m_windSpeed = 0.2f;
	m_windStrength = 1.0f;
}

void Quadtree::Build(HeightMap *pHeightMap, Vec3 *pVerts, Vec2 *pTexCoords, int numVerts)
{
	m_pHeightmap = pHeightMap;

	m_pNode = new Node(this);

	m_pNode->CreateNode(pVerts, pTexCoords, numVerts);


	int numEndNodes = pow(4.0f, g_maxSubdivisions);

	int subDivisionWidth = (sqrtf(numVerts) + sqrtf(numEndNodes) - 1) / sqrtf(numEndNodes);
	int numSubDivisionVerts = subDivisionWidth * subDivisionWidth;
	
	
	m_pLeftVertex	= new float[numSubDivisionVerts];
	m_pRightVertex	= new float[numSubDivisionVerts];
	m_pTopVertex	= new float[numSubDivisionVerts];
	m_pBottomVertex	= new float[numSubDivisionVerts];

	memset(m_pLeftVertex, 0, sizeof(float) * numSubDivisionVerts);
	memset(m_pRightVertex, 0, sizeof(float) * numSubDivisionVerts);
	memset(m_pTopVertex, 0, sizeof(float) * numSubDivisionVerts);
	memset(m_pBottomVertex, 0, sizeof(float) * numSubDivisionVerts);

	for(int i = 0; i < numSubDivisionVerts; i++)
	{
		if(i < subDivisionWidth)
			m_pLeftVertex[i] = 1.0f;
		else if(i > numSubDivisionVerts - subDivisionWidth)
			m_pRightVertex[i] = 1.0f;
		else if(i % subDivisionWidth == 0)
			m_pTopVertex[i] = 1.0f;
		else if((i + 1) % subDivisionWidth == 0)
			m_pBottomVertex[i] = 1.0f;
	}

	// Build an array with pointers to the nodes that contain the vertices 
	// to search for the neighbour node of each end node

	Node **pEndNodes;

	pEndNodes = (Node**) malloc(numEndNodes * sizeof(Node *));
	
	m_pNode->FindEndNodes(pEndNodes, m_pNode);
	
	m_pNode->FindNeighbEndNodes(pEndNodes, m_pNode, numEndNodes);

	delete [] pEndNodes;

	//Model model;
	m_models = new Model;
	g_importer3DS.Import3DS(&model, "data\\models\\3ds__tree2.3ds");
	*m_models = model;
	//AddModel(model);
}

void Quadtree::Render(Frustum frustum, Vec3 &eyePos)
{
	m_numTrisLastRendered = 0;

	if(m_useGeomipmaps)
		m_pNode->Render(m_pNode, frustum, eyePos);
	else
		m_pNode->RenderStaticLod(m_pNode, frustum, eyePos, 0);
}

void Quadtree::RenderStaticLOD(Frustum frustum, Vec3 &eyePos, int lod)
{
	m_numTrisLastRendered = 0;
	m_pNode->RenderStaticLod(m_pNode, frustum, eyePos, lod);
}

void Quadtree::IncNumTrisRendered(int numTrianglesRendered)
{
	m_numTrisLastRendered += numTrianglesRendered;
}


void Quadtree::AddModel(Model &model)
{
//	m_models.push_back(model);
	//m_models = new Model
}
/*
const std::vector<Model> Quadtree::GetModelList() const
{
	//return m_models;
}*/