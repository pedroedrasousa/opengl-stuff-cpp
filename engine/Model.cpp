#include <windows.h>
#include <gl\gl.h>
#include "gl\glu.h"
#include "gl\glext.h"

#include "Model.h"
#include "OpenGLExt.h"
#include "Shader.h"
#include "Camera.h"


#define BINORMAL_ARRAY 3
#define TANGENT_ARRAY 4

#define USE_SHADOW_VERTEX_SHADER

extern GLuint texNormalisationCubeMap;
extern GLuint texCubeMap;

#define BACK_EXTRUDE_LENGHT 5000
#define FRONT_EXTRUDE_LENGHT 0

Model::Model()
{

}

Model::~Model()
{

}

void Model::render(Mat4 &mPose, Frustum *frustum)
{

	glPushAttrib(GL_ENABLE_BIT);

	glPushMatrix();
	glMultMatrixf(mPose);


	for(int i = 0; i < m_NumMeshes; i++)
	{
		if(m_pMesh.size() <= 0) break;

		Mesh *pMesh = &m_pMesh[i];

		if(frustum)
		{
			//if(!frustum->SphereInFrustum(pose.x, pose.y, pose.z, pMesh->m_BoundingSphere.radius))
				//continue;

			if(!frustum->ParallelepipedInFrustum(mPose, pMesh->m_BoundingBox.v))
				continue;
		}

		if(m_NumMaterials > 0)
		{
				//glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
				
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, m_pMaterial[pMesh->m_MaterialID].difuseTex.GetID() );
		}

		pMesh->renderTriangles();

	}



	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_TEXTURE_2D);

		glPopMatrix();



	glPopAttrib();
}

void Mesh::renderTriangles()
{
	glVertexPointer(3, GL_FLOAT, 0, this->m_pVerts);
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_FLOAT, sizeof(t_TangentSpace), this->m_pTangentSpace->normal);
	glEnableClientState(GL_NORMAL_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, this->m_pTexVerts);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexAttribPointerARB(BINORMAL_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof(t_TangentSpace), this->m_pTangentSpace->binormal);
	glEnableVertexAttribArrayARB(BINORMAL_ARRAY);

	glVertexAttribPointerARB(TANGENT_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof(t_TangentSpace), this->m_pTangentSpace->tangent);
	glEnableVertexAttribArrayARB(TANGENT_ARRAY);
	
	glDrawElements(GL_TRIANGLES, this->m_NumFaces * 3, GL_UNSIGNED_INT, this->m_pVertIndex);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableVertexAttribArrayARB(BINORMAL_ARRAY);
	glDisableVertexAttribArrayARB(TANGENT_ARRAY);
}


/*
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
*/

void Model::computeTangentSpace()
{
	for(int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		if(m_pMesh.size() <= 0)break;

		Mesh *p_Mesh = &m_pMesh[iMesh];

		t_TangentSpace *p_TmpTangentSpace = new t_TangentSpace[p_Mesh->m_NumFaces];
		memset(p_TmpTangentSpace, 0, sizeof(t_TangentSpace) * p_Mesh->m_NumFaces);

		p_Mesh->m_pTangentSpace = new t_TangentSpace [p_Mesh->m_NumVerts];

		for(int i = 0; i < p_Mesh->m_NumFaces; i++)
		{
			int i1 = p_Mesh->m_pFaces[i].vertIndex[0];
			int i2 = p_Mesh->m_pFaces[i].vertIndex[1];
			int i3 = p_Mesh->m_pFaces[i].vertIndex[2];

			Vec3 p1 = p_Mesh->m_pVerts[i1];
			Vec3 p2 = p_Mesh->m_pVerts[i2];
			Vec3 p3 = p_Mesh->m_pVerts[i3];

			Vec3 v1 = p2 - p1;
			Vec3 v2 = p3 - p1;

			p_TmpTangentSpace[i].normal = Cross(v1, v2);

			Vec2 w1 = p_Mesh->m_pTexVerts[i1];
			Vec2 w2 = p_Mesh->m_pTexVerts[i2];
			Vec2 w3 = p_Mesh->m_pTexVerts[i3];

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0f / (s1 * t2 - s2 *t1);
			
			Vec3 sdir((t2 * v1.x - t1 * v2.x) * r, (t2 * v1.y - t1 * v2.y) * r, (t2 * v1.z - t1 * v2.z) * r);
			Vec3 tdir((s1 * v2.x - s2 * v1.x) * r, (s1 * v2.y - s2 * v1.y) * r, (s1 * v2.z - s2 * v1.z) * r);

			p_TmpTangentSpace[i].tangent += sdir;
			p_TmpTangentSpace[i].tangent.Normalize();

			p_TmpTangentSpace[i].binormal += tdir;
			p_TmpTangentSpace[i].binormal = -Normalize(p_TmpTangentSpace[i].binormal);
		}

		Vec3 normalSum = Vec3(0.0, 0.0, 0.0);
		Vec3 binormalSum = Vec3(0.0, 0.0, 0.0);
		Vec3 tangentSum = Vec3(0.0, 0.0, 0.0);
		int shared = 0;
		int sharedn = 0;

		for (int i = 0; i < p_Mesh->m_NumVerts; i++)
		{
			DWORD smoothGroup;
			for (int j = 0; j < p_Mesh->m_NumFaces; j++)
			{
				if (p_Mesh->m_pFaces[j].vertIndex[0] == i || 
					p_Mesh->m_pFaces[j].vertIndex[1] == i || 
					p_Mesh->m_pFaces[j].vertIndex[2] == i)
				{
					smoothGroup  = p_Mesh->m_pFaces[j].smoothGroup;
					normalSum	+= p_TmpTangentSpace[j].normal;
					binormalSum	+= p_TmpTangentSpace[j].binormal;
					tangentSum  += p_TmpTangentSpace[j].tangent;
					shared++;
					sharedn++;

				}
			}

			Vec3 vertPos = p_Mesh->m_pVerts[i];

		for (int j = 0; j < p_Mesh->m_NumFaces; j++)
			{
				t_Face *p_Face = &p_Mesh->m_pFaces[j];

				for(int k = 0; k < 3; k++)
				{
					if (p_Face->vertIndex[k] != i && p_Mesh->m_pVerts[ p_Face->vertIndex[k] ] == vertPos && p_Face->smoothGroup == smoothGroup)
					{
						//smoothGroup  = p_Mesh->m_pFaces[j].smoothGroup;
						normalSum	+= p_TmpTangentSpace[j].normal;
						binormalSum	+= p_TmpTangentSpace[j].binormal;
						tangentSum  += p_TmpTangentSpace[j].tangent;
						sharedn++;
					}
				}

			}

			p_Mesh->m_pTangentSpace[i].normal = normalSum / (float)sharedn;
			p_Mesh->m_pTangentSpace[i].normal.Normalize();
			p_Mesh->m_pTangentSpace[i].binormal = binormalSum / (float)sharedn;
			p_Mesh->m_pTangentSpace[i].binormal.Normalize();
			p_Mesh->m_pTangentSpace[i].tangent = tangentSum / (float)sharedn;
			p_Mesh->m_pTangentSpace[i].tangent.Normalize();

			normalSum	= Vec3(0.0, 0.0, 0.0);
			binormalSum	= Vec3(0.0, 0.0, 0.0);
			tangentSum	= Vec3(0.0, 0.0, 0.0);
			shared = 0;
		}

		delete [] p_TmpTangentSpace;
	}
}

void Model::computeConectivity()
{
	int iP1a, iP2a, iP1b, iP2b;
	Vec3 p1a, p2a, p1b, p2b;

	for(int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		Mesh *p_Mesh = &m_pMesh[iMesh];

		for(int i = 0; i < p_Mesh->m_NumFaces; i++)
		{
			p_Mesh->m_pFaces[i].neighbIndex[0] = -1;
			p_Mesh->m_pFaces[i].neighbIndex[1] = -1;
			p_Mesh->m_pFaces[i].neighbIndex[2] = -1;
		}
	}

	for(int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		Mesh *p_Mesh = &m_pMesh[iMesh];

		for(int iFaceA = 0; iFaceA < p_Mesh->m_NumFaces; iFaceA++)
		{
			for(int iEdgeA = 0; iEdgeA < 3; iEdgeA++)
			{
				if(p_Mesh->m_pFaces[iFaceA].neighbIndex[iEdgeA] == -1)
				{
					for(int iFaceB = iFaceA + 1; iFaceB < p_Mesh->m_NumFaces; iFaceB++)
					{
						for(int iEdgeB = 0; iEdgeB < 3; iEdgeB++)
						{
							iP1a = iEdgeA;
							iP2a = (iEdgeA + 1) % 3;
							iP1b = iEdgeB;
							iP2b = (iEdgeB + 1) % 3;

							p1a = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFaceA].vertIndex[iP1a] ];
							p2a = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFaceA].vertIndex[iP2a] ];
							p1b = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFaceB].vertIndex[iP1b] ];
							p2b = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFaceB].vertIndex[iP2b] ];

							if( ((p1a == p1b) && (p2a == p2b)) || ((p1a == p2b) && (p2a == p1b)) )
							{
								p_Mesh->m_pFaces[iFaceA].neighbIndex[iEdgeA] = iFaceB;	  
								p_Mesh->m_pFaces[iFaceB].neighbIndex[iEdgeB] = iFaceA;
							}
						}
					}
				}
			}
		}
	}
}

void Model::computePlanes()
{
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		Mesh *p_Mesh = &m_pMesh[iMesh];

		for (int iFace = 0; iFace < p_Mesh->m_NumFaces; iFace++)
		{
			Vec3 p1, p2, p3;

			p1 = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFace].vertIndex[0] ];
			p2 = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFace].vertIndex[1] ];
			p3 = p_Mesh->m_pVerts[ p_Mesh->m_pFaces[iFace].vertIndex[2] ];

			p_Mesh->m_pFaces[iFace].plane.Compute(p1, p2, p3);
		}
	}
}

void Model::computeBoundingBox()
{
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		Mesh *p_Mesh = &m_pMesh[iMesh];

		float maxX = -9999;
		float maxY = -9999;
		float maxZ = -9999;
		
		float minX = 9999;
		float minY = 9999;
		float minZ = 9999;

        // Bounding box
		for (int iFace = 0; iFace < p_Mesh->m_NumFaces; iFace++)
		{
			t_Face *p_Face = &m_pMesh[iMesh].m_pFaces[iFace];

			for(int iVertex = 0; iVertex < 3; iVertex++)
			{
				if(p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].x > maxX)
					maxX = p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].x;
				if(p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].y > maxY)
					maxY = p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].y;
				if(p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].z > maxZ)
					maxZ = p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].z;
							
				if(p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].x < minX)
					minX = p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].x;
				if(p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].y < minY)
					minY = p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].y;
				if(p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].z < minZ)
					minZ = p_Mesh->m_pVerts[ p_Face->vertIndex[iVertex] ].z;
			}
		}

		p_Mesh->m_BoundingBox.v[0] = Vec3(maxX, maxY, maxZ);
		p_Mesh->m_BoundingBox.v[1] = Vec3(maxX, maxY, minZ);
		p_Mesh->m_BoundingBox.v[2] = Vec3(maxX, minY, maxZ);
		p_Mesh->m_BoundingBox.v[3] = Vec3(maxX, minY, minZ);
		p_Mesh->m_BoundingBox.v[4] = Vec3(minX, maxY, maxZ);
		p_Mesh->m_BoundingBox.v[5] = Vec3(minX, maxY, minZ);
		p_Mesh->m_BoundingBox.v[6] = Vec3(minX, minY, maxZ);
		p_Mesh->m_BoundingBox.v[7] = Vec3(minX, minY, minZ);


		// Bounding sphere
		Vec3 *p_Offset = &p_Mesh->m_BoundingSphere.offset;

		p_Offset->x = minX + (maxX - minX) / 2.0f;
		p_Offset->y = minY + (maxY - minY) / 2.0f;
		p_Offset->z = minZ + (maxZ - minZ) / 2.0f;

		p_Mesh->m_BoundingSphere.radius = 0.0f;

		for (int iFace = 0; iFace < p_Mesh->m_NumFaces; iFace++)
		{
			t_Face *p_Face = &m_pMesh[iMesh].m_pFaces[iFace];

			for(int iVertex = 0; iVertex < 3; iVertex++)
			{
				int index = p_Face->vertIndex[iVertex];

				if( (p_Mesh->m_pVerts[ index ] - *p_Offset).Length() > p_Mesh->m_BoundingSphere.radius)
					p_Mesh->m_BoundingSphere.radius = (p_Mesh->m_pVerts[ index ] - *p_Offset).Length();
			}

		}

	}
}

void Model::loadTextures()
{
	for (int i = 0; i < m_NumMaterials; i++)
	{
		if(strlen(m_pMaterial[i].strDifuseTexFile) > 0)
		{
			char filename[255];
			sprintf(filename, "data\\models\\%s", m_pMaterial[i].strDifuseTexFile);
			m_pMaterial[i].difuseTex.Load2D(filename);
		}

		if(strlen(m_pMaterial[i].strBumpTexFile) > 0)
		{
			char filename[255];
			sprintf(filename, "data\\models\\%s", m_pMaterial[i].strBumpTexFile);
			m_pMaterial[i].bumpTex.Load2D(filename);
		}
	}
}

void Model::renderBoundingSphereDebugLines(Mat4 const &mPose)
{
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		Mesh *p_Mesh = &m_pMesh[iMesh];

		glPushMatrix();
		glMultMatrixf(mPose.m);
		glTranslatef(p_Mesh->m_BoundingSphere.offset.x, p_Mesh->m_BoundingSphere.offset.y, p_Mesh->m_BoundingSphere.offset.z);


		GLUquadricObj * quadObj = gluNewQuadric ();
		gluQuadricDrawStyle (quadObj, GLU_LINE);
		//gluQuadricNormals (quadObj, GLU_SMOOTH); 
		//gluQuadricOrientation(quadObj,GLU_OUTSIDE);
		gluSphere(quadObj, p_Mesh->m_BoundingSphere.radius, 10, 10);	//unit sphere
		gluDeleteQuadric(quadObj);

		glPopMatrix();
	}
}


void Model::renderBoundingBoxDebugLines(Mat4 const &mPose)
{
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		Mesh *p_Mesh = &m_pMesh[iMesh];

		glColor3f(1.0f, 0.0f, 0.0f);
/*
		glBegin(GL_LINES);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.positiveZ);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.negativeZ);
				
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.negativeZ);
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.negativeZ);
			
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.negativeZ);
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.positiveZ);

			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.positiveZ);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.positiveZ);
		

			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.positiveZ);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.negativeZ);
				
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.negativeZ);
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.negativeZ);
			
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.negativeZ);
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.positiveZ);

			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.positiveZ);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.positiveZ);


			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.positiveZ);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.positiveZ);
			
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.negativeZ);
			glVertex3f(p_Mesh->m_BoundingBox.positiveX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.negativeZ);
			
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.positiveZ);
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.positiveZ);

			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.positiveY, p_Mesh->m_BoundingBox.negativeZ);
			glVertex3f(p_Mesh->m_BoundingBox.negativeX, p_Mesh->m_BoundingBox.negativeY, p_Mesh->m_BoundingBox.negativeZ);
		glEnd();
*/
		glColor3f(1.0f, 1.0f, 1.0f);

	}
}

void Model::renderTangentSpaceDebugLines()
{
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_COLOR_MATERIAL);

	glBegin(GL_LINES);

	for(int i = 0; i < m_NumMeshes; i++)
	{
		if(m_pMesh.size() <= 0) break;

		Mesh *p_Mesh = &m_pMesh[i];

		for(int j = 0; j < p_Mesh->m_NumVerts; j++)
		{
			Vec3 *p_Vertex = &p_Mesh->m_pVerts[j];
			Vec3 *p_Normal = &p_Mesh->m_pTangentSpace[j].normal;
			Vec3 *p_Biormal = &p_Mesh->m_pTangentSpace[j].binormal;
			Vec3 *p_Tangent = &p_Mesh->m_pTangentSpace[j].tangent;

			glColor3ub(255, 0, 0);
			glVertex3f(p_Vertex->x, p_Vertex->y, p_Vertex->z);
			glVertex3f(p_Vertex->x + p_Normal->x, p_Vertex->y + p_Normal->y, p_Vertex->z + p_Normal->z);
			
			glColor3ub(0, 255, 0);
			glVertex3f(p_Vertex->x, p_Vertex->y, p_Vertex->z);
			glVertex3f(p_Vertex->x + p_Biormal->x, p_Vertex->y + p_Biormal->y, p_Vertex->z + p_Biormal->z);
			
			glColor3ub(0, 0, 255);
			glVertex3f(p_Vertex->x, p_Vertex->y, p_Vertex->z);
			glVertex3f(p_Vertex->x + p_Tangent->x, p_Vertex->y + p_Tangent->y, p_Vertex->z + p_Tangent->z);
		}
	}

	glEnd();

	glPopAttrib();
	glColor3ub(255, 255, 255);
}