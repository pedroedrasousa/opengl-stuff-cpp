
#ifndef __PLANE_H__
#define __PLANE_H__

#include "Vector.h"

class Plane {
private:
public:
	float a, b, c, d;

	Plane::Plane() {};
	Plane::Plane(float a, float b, float c, float d);
	Plane::Plane(Vec3 &p1, Vec3 &p2, Vec3 &p3);
	Plane::~Plane() {};

	void Compute(Vec3 &p1, Vec3 &p2, Vec3 &p3);
	void Normalize();
	const Vec3 GetNormal() const;
	const Vec4 GetCoefficients() const;
	const float GetConstant() const;
};

#endif /*__PLANE_H__*/