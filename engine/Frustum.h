
#ifndef __FRUSTRUM_H__
#define __FRUSTRUM_H__

#include "Matrix.h"
#include "Vector.h"
#include "Plane.h"

class Frustum {
public:

	void ComputeFrustum(const Mat4 &mProjection, const Mat4 &mModelView);
	float DistToEye(const Vec3 &pos) const;
	bool PointInFrustum(const Vec3 &pos) const;
	float SphereInFrustum(const Vec3 &pos, float radius) const;
	bool CubeInFrustum(const Vec3 &pos, float size) const;
	bool ParallelepipedInFrustum(const Vec3 &pos, float width, float height, float depth) const;
	bool ParallelepipedInFrustum(const Vec3 &pos, const Vec3 *v) const;
	bool ParallelepipedInFrustum(const Mat4 &mPose, const Vec3 *v) const;

private:

	enum FrustumSide {RIGHT, LEFT, BOTTOM, TOP, BACK, FRONT};

	Plane m_Frustum[6];
};

#endif /*__FRUSTRUM_H__*/