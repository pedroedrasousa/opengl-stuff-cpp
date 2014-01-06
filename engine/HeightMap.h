
#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#define MAP_SIZE		1024
//#define STEP_SIZE		16
//#define HEIGHT_RATIO	1.0f

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#include "Vector.h"

class HeightMap {
public:
	HeightMap() {};
	~HeightMap() {};

	float GetHeight(int x, int y) const;
	float GetInterpolatedHeight(float x, float y) const;

	int BuildArray(Vec3 **kvertices, Vec2 **tex1Coords, Vec2 **tex2Coords);
	void LoadFromFile(const char *filename);

	void Render();
	
	void SetWidthRatio(float ratio);
	void SetHeightRatio(float ratio);
	void SetYOffset(float offset);

	int		*vertInd;
	Vec3	*vertices;

	int		numVerts;

private:
	unsigned char *data;

	int width;
    int height;


	float	heightRatio;
	int		step;
	float	widthRatio;
	float	yOffset;
};

#endif /*__HEIGHTMAP_H__*/