
#include <math.h>

#include "Matrix.h"
#include "Math.h"

//====================================================================================================
// Mat3
//====================================================================================================

Mat3 mat3_zero( 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f );

Mat3 mat3_identity( 1.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 1.0f );

Mat3 Mat3::Transpose() const
{
	return Mat3(
		m[0], m[1], m[2],
		m[3], m[4], m[5],
		m[6], m[7], m[8] );
}

void Mat3::TransposeSelf()
{
	(*this) = this->Transpose();
}

float Mat3::Determinant() const
{
	return	m[0] * m[4] * m[8]
		  + m[3] * m[7] * m[2]
		  + m[6] * m[1] * m[5]
		  - m[6] * m[4] * m[2]
		  - m[0] * m[7] * m[5]
		  - m[3] * m[1] * m[8];
}

Mat3 Mat3::Inverse() const
{
	Mat3 inverse;

	float invDet = 1.0f / Determinant();

	inverse[0] =  (m[4] * m[8] - m[7] * m[5]) * invDet;
	inverse[1] = -(m[1] * m[8] - m[7] * m[2]) * invDet;
	inverse[2] =  (m[1] * m[5] - m[4] * m[2]) * invDet;

	inverse[3] = -(m[3] * m[8] - m[6] * m[5]) * invDet;
	inverse[4] =  (m[0] * m[8] - m[6] * m[2]) * invDet;
	inverse[5] = -(m[0] * m[5] - m[3] * m[2]) * invDet;

	inverse[6] =  (m[3] * m[7] - m[6] * m[4]) * invDet;
	inverse[7] = -(m[0] * m[7] - m[6] * m[1]) * invDet;
	inverse[8] =  (m[0] * m[4] - m[3] * m[1]) * invDet;

	return inverse;
}

void Mat3::Scale(const float x, const float y, const float z)
{
	m[0] *= x; m[3] *= y; m[6] *= z;
	m[1] *= x; m[4] *= y; m[7] *= z;
	m[2] *= x; m[5] *= y; m[8] *= z;
}

void Mat3::BuildRotate(const Vec3 &axis, const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);
	Vec3 v = Normalize(axis);

	float xx = v.x * v.x;
	float yy = v.y * v.y;
	float zz = v.z * v.z;
	float xy = v.x * v.y;
	float yz = v.y * v.z;
	float zx = v.z * v.x;
	float xs = v.x * s;
	float ys = v.y * s;
	float zs = v.z * s;

	m[0] = (1.0f - c) * xx + c;  m[3] = (1.0f - c) * xy - zs; m[6] = (1.0f - c) * zx + ys;
	m[1] = (1.0f - c) * xy + zs; m[4] = (1.0f - c) * yy + c;  m[7] = (1.0f - c) * yz - xs;
	m[2] = (1.0f - c) * zx - ys; m[5] = (1.0f - c) * yz + xs; m[8] = (1.0f - c) * zz + c;
}

void Mat3::BuildRotate(const float x, const float y, const float z, const float angle)
{
	BuildRotate(Vec3(x, y, z), angle);
}

void Mat3::BuildRotateX(const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	m[0] = 1.0; m[3] = 0.0; m[6] = 0.0;
	m[1] = 0.0; m[4] = c;   m[7] = -s;
	m[2] = 0.0; m[5] = s;   m[8] = c;
}

void Mat3::BuildRotateY(const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	m[0] = c;   m[3] = 0.0; m[6] = s;
	m[1] = 0.0; m[4] = 1.0; m[7] = 0.0;
	m[2] = -s;  m[5] = 0.0; m[8] = c;
}

void Mat3::BuildRotateZ(const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	m[0] = c;   m[3] = -s;  m[6] = 0.0;
	m[1] = s;   m[4] = c;   m[7] = 0.0;
	m[2] = 0.0; m[5] = 0.0; m[8] = 1.0;
}

void Mat3::Rotate(const Vec3 &axis, const float angle)
{
	Mat3 tmp;
	
	tmp.BuildRotate(axis, angle);
	(*this) *= tmp;
}

void Mat3::Rotate(const float x, const float y, const float z, const float angle)
{
	Rotate(Vec3(x, y, z), angle);
}

void Mat3::RotateX(const float angle)
{
	Mat3 tmp;
	
	tmp.BuildRotateX(angle);
	(*this) *= tmp;
}

void Mat3::RotateY(const float angle)
{
	Mat3 tmp;
	
	tmp.BuildRotateY(angle);
	(*this) *= tmp;
}

void Mat3::RotateZ(const float angle)
{
	Mat3 tmp;
	
	tmp.BuildRotateZ(angle);
	(*this) *= tmp;
}

//====================================================================================================
// Mat4
//====================================================================================================

Mat4 mat4_zero( 0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f );

Mat4 mat4_identity( 1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f );

Mat4 Mat4::Transpose() const
{
	return Mat4(
		m[ 0], m[ 1], m[ 2], m[ 3],
		m[ 4], m[ 5], m[ 6], m[ 7],
		m[ 8], m[ 9], m[10], m[11],
		m[12], m[13], m[14], m[15] );
}

void Mat4::TransposeSelf()
{
	(*this) = this->Transpose();
}

float Mat4::Determinant() const
{
	return	m[0] * m[5] * m[10]
		  + m[4] * m[9] * m[2]
		  + m[8] * m[1] * m[6]
		  - m[8] * m[5] * m[2]
		  - m[4] * m[1] * m[10]
		  - m[0] * m[9] * m[6];
}

Mat4 Mat4::Inverse() const
{
	Mat4 inverse;

	float invDet = 1.0f / Determinant();

	inverse[0] =  (m[5] * m[10] - m[9] * m[6]) * invDet;
	inverse[1] = -(m[1] * m[10] - m[9] * m[2]) * invDet;
	inverse[2] =  (m[1] * m[6]  - m[5] * m[2]) * invDet;
	inverse[3] = 0.0;

	inverse[4] = -(m[4] * m[10] - m[8] * m[6]) * invDet;
	inverse[5] =  (m[0] * m[10] - m[8] * m[2]) * invDet;
	inverse[6] = -(m[0] * m[6]  - m[4] * m[2]) * invDet;
	inverse[7] = 0.0;

	inverse[8] =  (m[4] * m[9] - m[8] * m[5]) * invDet;
	inverse[9] = -(m[0] * m[9] - m[8] * m[1]) * invDet;
	inverse[10] = (m[0] * m[5] - m[4] * m[1]) * invDet;
	inverse[11] = 0.0;

	inverse[12] = -(m[12] * inverse[0] + m[13] * inverse[4] + m[14] * inverse[8]);
	inverse[13] = -(m[12] * inverse[1] + m[13] * inverse[5] + m[14] * inverse[9]);
	inverse[14] = -(m[12] * inverse[2] + m[13] * inverse[6] + m[14] * inverse[10]);
	inverse[15] = 1.0;

	return inverse;
}

void Mat4::InverseSelf()
{
	*this = this->Inverse();
}

void Mat4::Translate(const Vec3 &v)
{
	m[12] += (v.x * m[0] + v.y * m[4] + v.z * m[8]);
	m[13] += (v.x * m[1] + v.y * m[5] + v.z * m[9]);
	m[14] += (v.x * m[2] + v.y * m[6] + v.z * m[10]);
	m[15] += (v.x * m[3] + v.y * m[7] + v.z * m[11]);
}

void Mat4::Scale(const float x, const float y, const float z)
{
	m[0] *= x; m[4] *= y; m[ 8] *= z;
	m[1] *= x; m[5] *= y; m[ 9] *= z;
	m[2] *= x; m[6] *= y; m[10] *= z;
	m[3] *= x; m[7] *= y; m[11] *= z;
}

void Mat4::BuildRotate(const Vec3 &axis, const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);
	Vec3 v = Normalize(axis);

	float xx = v.x * v.x;
	float yy = v.y * v.y;
	float zz = v.z * v.z;
	float xy = v.x * v.y;
	float yz = v.y * v.z;
	float zx = v.z * v.x;
	float xs = v.x * s;
	float ys = v.y * s;
	float zs = v.z * s;

	m[0] = (1.0f - c) * xx + c;  m[4] = (1.0f - c) * xy - zs; m[ 8] = (1.0f - c) * zx + ys; m[12] = 0.0;
	m[1] = (1.0f - c) * xy + zs; m[5] = (1.0f - c) * yy + c;  m[ 9] = (1.0f - c) * yz - xs; m[13] = 0.0;
	m[2] = (1.0f - c) * zx - ys; m[6] = (1.0f - c) * yz + xs; m[10] = (1.0f - c) * zz + c;	m[14] = 0.0;
	m[3] = 0.0;					 m[7] = 0.0;				  m[11] = 0.0;					m[15] = 1.0;
}

void Mat4::BuildRotate(const float x, const float y, const float z, const float angle)
{
	BuildRotate(Vec3(x, y, z), angle);
}

void Mat4::BuildRotateX(const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	m[0] = 1.0; m[4] = 0.0; m[ 8] = 0.0; m[12] = 0.0;
	m[1] = 0.0; m[5] = c;	m[ 9] = -s;	 m[13] = 0.0;
	m[2] = 0.0; m[6] = s;	m[10] = c;   m[14] = 0.0;
	m[3] = 0.0; m[7] = 0.0; m[11] = 0.0; m[15] = 1.0;
}

void Mat4::BuildRotateY(const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	m[0] = c;   m[4] = 0.0; m[ 8] = s;   m[12] = 0.0;
	m[1] = 0.0; m[5] = 1.0; m[ 9] = 0.0; m[13] = 0.0;
	m[2] = -s;  m[6] = 0.0; m[10] = c;   m[14] = 0.0;
	m[3] = 0.0; m[7] = 0.0; m[11] = 0.0; m[15] = 1.0;
}

void Mat4::BuildRotateZ(const float angle)
{
	float rad = DEG2RAD(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	m[0] = c;   m[4] = -s;  m[ 8] = 0.0; m[12] = 0.0;
	m[1] = s;   m[5] = c;   m[ 9] = 0.0; m[13] = 0.0;
	m[2] = 0.0; m[6] = 0.0; m[10] = 1.0; m[14] = 0.0;
	m[3] = 0.0; m[7] = 0.0; m[11] = 0.0; m[15] = 1.0;
}

void Mat4::Rotate(const Vec3 &axis, const float angle)
{
	Mat4 tmp;
	
	tmp.BuildRotate(axis, angle);
	(*this) *= tmp;
}

void Mat4::Rotate(const float x, const float y, const float z, const float angle)
{
	Rotate(Vec3(x, y, z), angle);
}

void Mat4::RotateX(const float angle)
{
	Mat4 tmp;
	
	tmp.BuildRotateX(angle);
	(*this) *= tmp;
}

void Mat4::RotateY(const float angle)
{
	Mat4 tmp;
	
	tmp.BuildRotateY(angle);
	(*this) *= tmp;
}

void Mat4::RotateZ(const float angle)
{
	Mat4 tmp;
	
	tmp.BuildRotateZ(angle);
	(*this) *= tmp;
}

void Mat4::BuildProjection(float fov, float aspect, float zNear, float zFar)
{
	float left, right, top , bottom;
	float x, y, a, b, c, d;

	top		= zNear * (float)tan(fov * PI / 360.0f);
	bottom	= -top;
	left	= bottom * aspect;
	right	= top * aspect;

	x = float( (2.0 * zNear) / (right - left) );
	y = float( (2.0 * zNear) / (top - bottom) );
	a = float( (right + left) / (right - left) );
	b = float( (top + bottom) / (top - bottom) );
	c = float( -(zFar + zNear) / ( zFar - zNear) );
	d = float( -(2.0 * zFar * zNear) / (zFar - zNear) );

	m[0] = x;		m[4] = 0.0f;	m[ 8] = a;		m[12] = 0.0f;
	m[1] = 0.0f;	m[5] = y;		m[ 9] = b;		m[13] = 0.0f;
	m[2] = 0.0f;	m[6] = 0.0f;	m[10] = c;		m[14] = d;
	m[3] = 0.0f;	m[7] = 0.0f;	m[11] = -1.0f;	m[15] = 0.0f;
}

void Mat4::BuildViewMatrix(const Vec3 &eye, const Vec3 &center, const Vec3 &up)
{
	Vec3 x, y, z;

	z = eye - center;
	x = Cross(up, z);
	y = Cross(z, x);
    
	x.Normalize();
	y.Normalize();
	z.Normalize();

	m[0] = x[0]; m[4] = x[1]; m[ 8] = x[2]; m[12] = 0.0f;
	m[1] = y[0]; m[5] = y[1]; m[ 9] = y[2]; m[13] = 0.0f;
	m[2] = z[0]; m[6] = z[1]; m[10] = z[2]; m[14] = 0.0f;
	m[3] = 0.0f, m[7] = 0.0f; m[11] = 0.0f; m[15] = 1.0f;

	Translate(-eye);
}