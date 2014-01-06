
#ifndef __SKYDOME_H__
#define __SKYDOME_H__

#include "Vector.h"
#include "Matrix.h"

class Skydome {
public:
	Skydome() {};
	~Skydome() {};

	void Init(float widthScale, float heightScale, int segments, int vertAngle, int gradientTexStartAngle, int gradientTexEndAngle, const Mat4 &textureMatrix);
	void Render() const;
	void RenderCubeMap() const;

private:

	int m_numTriStrips;

    bool m_bSouthPoleTriFan;

	int m_numTriStripVerts;
	int m_numTriFanVerts;

	Vec3 *m_pVerts;
	Vec3 *m_pNormals;
	Vec2 *m_pTexCoords;

	int *m_pTriStripsVertInd;
	int *m_pNorthFanVertInd;
    int *m_pSouthFanVertInd;

	float *m_pTexCoords1D;
};

#endif /*__SKYDOME_H__*/