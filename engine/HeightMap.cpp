
#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include "gl\wglext.h"

#include "gl\glut.h"

#include "HeightMap.h"
#include "Image.h"
#include "Plane.h"

float HeightMap::GetHeight(int x, int y) const
{
	if(!data)
		return 0.0f;

	//x = x % width;
	//y = y % height;

	unsigned char byte = data[x + (y * height)];//??????????

	return float( data[x + (y * height)] * heightRatio ) + yOffset;
}

float HeightMap::GetInterpolatedHeight(float x, float y) const
{
	if(!data)
		return 0.0f;

	float v1x, v1y;
	float h1, h2, h3, h4;
	int xData, yData;
	Vec3 v1, v2, v3;
	float v1v3Lenght, v1v4Lenght;

	v1x = floor(x / widthRatio) * widthRatio;
	v1y = floor(y / widthRatio) * widthRatio;

	xData = (int)(v1x / widthRatio) + width / 2;
	yData = (int)(v1y / widthRatio) + width / 2;

	if( xData >= width || xData <= -width || yData >= height || yData <= -height)
		return 0.0f;

	h1 = float( data[xData + yData * height] * heightRatio ) + yOffset;
	h4 = float( data[xData + 1 + (yData + 1) * height] * heightRatio ) + yOffset;

	v1 = Vec3(v1x, h1, v1y);
	v2 = Vec3(v1x + widthRatio, h4, v1y + widthRatio);

	v1v3Lenght = ( Vec2(x, y) - Vec2(v1x + widthRatio, v1y) ).Length();
	v1v4Lenght = ( Vec2(x, y) - Vec2(v1x, v1y + widthRatio) ).Length();

	if(v1v3Lenght < v1v4Lenght)
	{
		h2 = float( data[xData + 1 + (yData * height)] * heightRatio ) + yOffset;
		v3 = Vec3(v1x + widthRatio, h2, v1y);
	}
	else
	{
		h3 = float( data[xData + ((yData + 1) * height)] * heightRatio ) + yOffset;
		v3 = Vec3(v1x, h3, v1y + widthRatio);
	}

	Plane plane;
	plane.Compute(v1, v2, v3);

	return -(plane.GetNormal().x * x + plane.GetNormal().z * y + plane.GetConstant() ) / plane.GetNormal().y;
}

void HeightMap::LoadFromFile(const char *filename)
{
	Image image;

	if( !image.Load(filename) )
		return;

	if(image.GetNumChannels() != 1)
		return;

	data = new unsigned char[image.GetWidth() * image.GetHeight()];
	memcpy(data, image.GetPixels(), image.GetWidth() * image.GetHeight() * sizeof(unsigned char));

	this->width = image.GetWidth();
	this->height = image.GetHeight();

	this->step = 1;
}

int HeightMap::BuildArray(Vec3 **kvertices, Vec2 **tex1Coords, Vec2 **tex2Coords)
{
	int iVert = 0;
	int numVerts = 0;
    
	if(!data)
		return 0;

	this->numVerts = width * height * 2 - height - height;
	vertInd = new int[this->numVerts];

	vertices	= new Vec3[width * height];
	*tex1Coords = new Vec2[width * height];
	*tex2Coords = new Vec2[width * height];
	
	/*vertices = new Vec3[6 * (width+1000) * (height+1000) / (2 * step)];
	*tex1Coords = new Vec2[6 * (width+1000) * (height+1000) / (2 * step)];
	*tex2Coords = new Vec2[6 * (width+1000) * (height+1000) / (2 * step)];*/
	
		
	//memset(*texCoords, 0, 6 * (width+1000) * (height+1000) / (2 * step) * sizeof(Vec2));
	
	unsigned long index = 0;


	for (int i = 0; i < width; i += step)
	{
		for (int j = 0; j < height; j += step)
		{

			float x = (i - width / 2) * widthRatio;
			float y = GetHeight(i, j); 
			float z = (j - width / 2) * widthRatio;

			(vertices)[iVert]		= Vec3(x , y, z);
			(*tex1Coords)[iVert]	= Vec2( (float)i / (float)width, (float)j / (float)height);
			(*tex2Coords)[iVert]	= Vec2( (float)i / 8.0f, (float)j / 8.0f ); //???????

			iVert++;
			numVerts++;
		}
	}

	iVert = 0;

	bool direction = false;

	for (int i = 0; i < width; i++)
	{
		direction = !direction;

		for (int j = 0; j < height; j++)
		{
			if(direction == true && i < width - 1)
			{
				vertInd[index++] = iVert + height;
				vertInd[index++] = iVert;

				iVert++;
			}
			else if(direction == false && i < width - 1)
			{
				vertInd[index++] = (iVert / height) * height + height - (iVert % height) - 1;
				vertInd[index++] = (iVert / height) * height + height - (iVert % height) + height - 1;

				iVert++;
			}

		}
	}

	

	return numVerts;
}

void HeightMap::Render()
{
	glBegin(GL_TRIANGLE_STRIP);

	for (int i = 0; i < numVerts; i++)
	{
		glColor3f(1.0f, i % 2, 0.0f);
			glVertex3f((vertices)[vertInd[i]].x, (vertices)[vertInd[i]].y, (vertices)[vertInd[i]].z);
	}

	glEnd();

}

void HeightMap::SetWidthRatio(float ratio)
{
	widthRatio = ratio;
}

void HeightMap::SetHeightRatio(float ratio)
{
	heightRatio = ratio;
}

void HeightMap::SetYOffset(float offset)
{
	this->yOffset = offset;
}
