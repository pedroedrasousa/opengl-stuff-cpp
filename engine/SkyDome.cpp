
#include <windows.h>
#include <gl\gl.h>
#include <assert.h>

#include "Skydome.h"
#include "OpenGLExt.h"
#include "Math.h"

void Skydome::Init(float widthScale, float heightScale, int segments, int vertAngle, int gradientTexStartAngle, int gradientTexEndAngle, const Mat4 &textureMatrix)
{
	assert(vertAngle >= 0 && vertAngle <= 180);
	assert(segments >= 4);

	int numVerts;
	int parallels = segments / 2;
	float horizAngleStep = 2 * PI / (float)segments;
	float vertAngleStep = DEG2RAD(vertAngle * 2) / (float)segments;

	// Check if the sphere has a south pole
	// to be rendered using a triangle fan
    if(vertAngle == 180)
	{
		m_bSouthPoleTriFan = true;
		parallels--;
		numVerts = segments * parallels + 2;
	}
	else
	{
		m_bSouthPoleTriFan = false;
		numVerts = segments * parallels + 1;
	}

	int numVertsPerTriStrip = 2 * segments + 2;

	m_numTriStrips		= parallels - 1;
	m_numTriStripVerts	= m_numTriStrips * numVertsPerTriStrip;
	m_numTriFanVerts	= segments + 2;
	
	m_pVerts		= new Vec3[numVerts];
	m_pNormals		= new Vec3[numVerts];
	m_pTexCoords	= new Vec2[numVerts];
	m_pTexCoords1D	= new float[numVerts];

	// North pole - Top vertex
    m_pVerts[0].x = 0.0f;
    m_pVerts[0].y = 1.0f;
    m_pVerts[0].z = 0.0f;

	// South pole - Bottom vertex
	if(m_bSouthPoleTriFan)
	{
		m_pVerts[numVerts - 1].x = 0.0f;
		m_pVerts[numVerts - 1].y = -1.0f;
		m_pVerts[numVerts - 1].z = 0.0f;
	}

	// Compute the sphere vertices
	for(int i = 0; i < parallels; i++)
    {
        for(int j = 0; j < segments; ++j)
        {
            int vertex = 1 + i * segments + j; // Current vertex index

			m_pVerts[vertex].x = sinf(vertAngleStep * (i + 1)) * cosf(horizAngleStep * j);
            m_pVerts[vertex].y = cosf(vertAngleStep * (i + 1));
            m_pVerts[vertex].z = sinf(vertAngleStep * (i + 1)) * sinf(horizAngleStep * j);
        }
    }

	// Compute normals pointing out
	for(int i = 0; i < numVerts; i++)
	{
		m_pNormals[i] = m_pVerts[i];
	}

	// Give it the desired dimensions
	for(int i = 0; i < numVerts; i++)
	{
		m_pVerts[i].x *= widthScale;
		m_pVerts[i].z *= widthScale;
		m_pVerts[i].y *= heightScale;
	}

	// North triangle fan indexes
	m_pNorthFanVertInd = new int[segments + 2];
    m_pNorthFanVertInd[0] = 0;
    for(int i = 0; i < segments; i++)
        m_pNorthFanVertInd[i + 1] = i + 1;
    m_pNorthFanVertInd[segments + 1] = 1;

	// South triangle fan indexes
	if(m_bSouthPoleTriFan)
	{
		m_pSouthFanVertInd = new int[segments + 2];
		m_pSouthFanVertInd[0] = numVerts - 1;
		for(int i = 0; i < segments; i++)
			m_pSouthFanVertInd[segments - i] = numVerts - segments - 1 + i;
		m_pSouthFanVertInd[segments + 1] = numVerts - 2;
	}

	if(parallels > 1)
    {
		m_pTriStripsVertInd = new int [m_numTriStripVerts];
    }
    else
	{
        m_pTriStripsVertInd = NULL;
	}

	// Triangle strips indexes
	for(int i = 0; i < parallels - 1; i++) 
    {
        for(int j = 0; j < segments; j++)
        {
            m_pTriStripsVertInd[i * numVertsPerTriStrip + 2 * j]     = 1 + segments * i + j;
            m_pTriStripsVertInd[i * numVertsPerTriStrip + 2 * j + 1] = 1 + segments * (i + 1) + j;
        }

		// Repeat the first two vertices to link the two edges of the strip
        m_pTriStripsVertInd[i * numVertsPerTriStrip + 2 * segments    ] = 1 + segments * i;
        m_pTriStripsVertInd[i * numVertsPerTriStrip + 2 * segments + 1] = 1 + segments * (i + 1);
    }

	// Compute texture coordinates based on textureMatrix argument
	for(int i = 0; i < numVerts; i++)
	{
		Vec4 tmpTexCoords = textureMatrix * Vec4(m_pVerts[i], 1.0f);

		m_pTexCoords[i].x = tmpTexCoords.x / tmpTexCoords.w;
		m_pTexCoords[i].y = tmpTexCoords.y / tmpTexCoords.w;
	}

    // Compute 1D texture coordinates based on gradientTexStartAngle and gradientTexEndAngle arguments
	for(int i = 0; i < numVerts; i++)
	{
		int angle = (int)RAD2DEG( atan2f( Length( Vec2(m_pVerts[i].x ,m_pVerts[i].z) ) , m_pVerts[i].y) );
		if(angle < gradientTexStartAngle)
			m_pTexCoords1D[i] = 1.0f;
		else if(angle > gradientTexEndAngle)
			m_pTexCoords1D[i] = 0.0f;
		else
			m_pTexCoords1D[i] = ((float)angle - (float)gradientTexEndAngle) / ((float)gradientTexStartAngle - (float)gradientTexEndAngle);
	}
}

void Skydome::Render() const
{	
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glTexCoordPointer(2, GL_FLOAT, 0, m_pTexCoords);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	//glTexCoordPointer(2, GL_FLOAT, 0, m_pTexCoords);
	glTexCoordPointer(1, GL_FLOAT, 0, m_pTexCoords1D);

	glVertexPointer(3, GL_FLOAT, 0, m_pVerts);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// North pole
	glDrawElements(GL_TRIANGLE_FAN, m_numTriFanVerts, GL_UNSIGNED_INT, m_pNorthFanVertInd);

	// South pole
	if(m_bSouthPoleTriFan)
		glDrawElements(GL_TRIANGLE_FAN, m_numTriFanVerts, GL_UNSIGNED_INT, m_pSouthFanVertInd);

	// Strips
	glDrawElements(GL_TRIANGLE_STRIP, m_numTriStripVerts, GL_UNSIGNED_INT, m_pTriStripsVertInd);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

// Render the skydome using the normals as tex coords for cube mapping
void Skydome::RenderCubeMap() const
{	
	glVertexPointer(3, GL_FLOAT, 0, m_pVerts);
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, 0, m_pNormals);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// North pole
	glDrawElements(GL_TRIANGLE_FAN, m_numTriFanVerts, GL_UNSIGNED_INT, m_pNorthFanVertInd);

	// South pole
	if(m_bSouthPoleTriFan)
		glDrawElements(GL_TRIANGLE_FAN, m_numTriFanVerts, GL_UNSIGNED_INT, m_pSouthFanVertInd);

	// Strips
	glDrawElements(GL_TRIANGLE_STRIP, m_numTriStripVerts, GL_UNSIGNED_INT, m_pTriStripsVertInd);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}