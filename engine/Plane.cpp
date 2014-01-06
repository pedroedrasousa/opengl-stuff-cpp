
#include "Plane.h"

Plane::Plane(float a, float b, float c, float d):
a(a), b(b), c(c)
{
}

Plane::Plane(Vec3 &p1, Vec3 &p2, Vec3 &p3)
{
	this->Compute(p1, p2, p3);
}

void Plane::Compute(Vec3 &p1, Vec3 &p2, Vec3 &p3)
{
	Vec3 vNormal = Cross(p2 - p1, p3 - p2);
	vNormal.Normalize();

	this->a = vNormal.x;
	this->b = vNormal.y;
	this->c = vNormal.z;

	this->d = -(this->a * p1.x + this->b * p1.y + this->c * p1.z);
}

const Vec3 Plane::GetNormal() const
{
	return Vec3(this->a, this->b, this->c);
}

const Vec4 Plane::GetCoefficients() const
{
	return Vec4(this->a, this->b, this->c, this->d);
}

const float Plane::GetConstant() const
{
	return this->d;
}

void Plane::Normalize()
{
	Vec3 v(this->a,this->b, this->c);

	float magnitude = Length(this->GetNormal());

	this->a /= magnitude;
	this->b /= magnitude;
	this->c /= magnitude;
	this->d /= magnitude; 
}