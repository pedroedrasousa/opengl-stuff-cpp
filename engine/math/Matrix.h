
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <memory.h>
#include <assert.h>

#include "Vector.h"

#define MATRIX_EPSILON 1e-6

//====================================================================================================
// Mat3
//====================================================================================================

class Mat3
{
public:

	float m[9];

	Mat3() {};

	explicit Mat3(	const float f0,  const float f1,  const float f2,
					const float f3,  const float f4,  const float f5,
					const float f6,  const float f7,  const float f8 );
	explicit Mat3(const float src[9]);

	operator float *();
	operator const float *() const;

	Mat3 &	operator=(Mat3 &a);
	Mat3	operator+(const Mat3 &a) const;
	Mat3 &	operator+=(const Mat3 &a);
	Mat3	operator-(const Mat3 &a) const;
	Mat3 &	operator-=(const Mat3 &a);
	Mat3	operator*(const Mat3 &a) const;
	Mat3 &	operator*=(const Mat3 &a);

	Mat3	operator*(const float a) const;
	Mat3 &	operator*=(const float a);
	Mat3	operator/(const float a) const;
	Mat3 &	operator/=(const float a);

	Vec4	operator*(const Vec4 &a) const;
	Vec3	operator*(const Vec3 &a) const;

	friend Mat3		operator*(const float f, const Mat3 &m);
	friend Vec4		operator*(const Vec4 &v, const Mat3 &m);
	friend Vec3		operator*(const Vec3 &v, const Mat3 &m);
	friend Vec4 &	operator*=(Vec4 &v, const Mat3 &m);
	friend Vec3 &	operator*=(Vec3 &v, const Mat3 &m);

	bool Compare(const Mat3 &a) const;
	bool Compare(const Mat3 &a, const float epsilon) const;
	bool operator==(const Mat3 &a) const;
	bool operator!=(const Mat3 &a) const;

	Vec3	GetRow(int row) const;
	Vec3	GetColumn(int column) const;

	void	Zero();
	void	Identity();
	bool	IsIdentity(const float epsilon = MATRIX_EPSILON) const;
    Mat3	Transpose() const;
	void	TransposeSelf();
	float	Determinant() const;
	Mat3	Inverse() const;
	void	InverseSelf();
	void	Scale(const float x, const float y, const float z);

	void	BuildRotate(const Vec3 &axis, const float angle);
	void	BuildRotate(const float x, const float y, const float z, const float angle);
	void	BuildRotateX(const float angle);
	void	BuildRotateY(const float angle);
	void	BuildRotateZ(const float angle);
	void	Rotate(const Vec3 &axis, const float angle);
	void	Rotate(const float x, const float y, const float z, const float angle);
	void	RotateX(const float angle);
	void	RotateY(const float angle);
	void	RotateZ(const float angle);
};

extern Mat3 mat3_zero;
extern Mat3 mat3_identity;

__forceinline Mat3::Mat3(	const float f0,  const float f1,  const float f2,
							const float f3,  const float f4,  const float f5,
							const float f6,  const float f7,  const float f8 )
{	
	m[0] = f0;	m[3] = f3;	m[6] = f6;
	m[1] = f1;	m[4] = f4;	m[7] = f7;
	m[2] = f2;	m[5] = f5;	m[8] = f8;
};

__forceinline Mat3::Mat3(const float src[9])
{
	memcpy(m, src, 9 * sizeof(float));
};

__forceinline Mat3::operator float *()
{
	return m;
};
	
__forceinline Mat3::operator const float *() const
{
	return m;
};

__forceinline Mat3 &Mat3::operator=(Mat3 &a)
{
	memcpy(m, a.m, sizeof(float) * 9);
	return *this;
};

__forceinline Mat3 Mat3::operator+(const Mat3 &a) const
{
	return Mat3(
		m[0] + a.m[0], m[3] + a.m[3], m[6] + a.m[6],
		m[1] + a.m[1], m[4] + a.m[4], m[7] + a.m[7],
		m[2] + a.m[2], m[5] + a.m[5], m[8] + a.m[8] );
}

__forceinline Mat3 &Mat3::operator+=(const Mat3 &a)
{
	return *this = (*this) + a;
}	

__forceinline Mat3 Mat3::operator-(const Mat3 &a) const
{
	return Mat3(
		m[0] - a.m[0], m[3] - a.m[3], m[6] - a.m[6],
		m[1] - a.m[1], m[4] - a.m[4], m[7] - a.m[7],
		m[2] - a.m[2], m[5] - a.m[5], m[8] - a.m[8] );
}

__forceinline Mat3 &Mat3::operator-=(const Mat3 &a)
{
	return *this = (*this) - a;
}

__forceinline Mat3 Mat3::operator*(const Mat3 &a) const
{
	return Mat3(
			m[0] * a[0] + m[3] * a[1] + m[6] * a[2],
			m[1] * a[0] + m[4] * a[1] + m[7] * a[2],
			m[2] * a[0] + m[5] * a[1] + m[8] * a[2],
			m[0] * a[3] + m[3] * a[4] + m[6] * a[5],
			m[1] * a[3] + m[4] * a[4] + m[7] * a[5],
			m[2] * a[3] + m[5] * a[4] + m[8] * a[5],
			m[0] * a[6] + m[3] * a[7] + m[6] * a[8],
			m[1] * a[6] + m[4] * a[7] + m[7] * a[8],
			m[6] * a[6] + m[7] * a[7] + m[8] * a[8] );
}

__forceinline Mat3 &Mat3::operator*=(const Mat3 &a)
{
	return *this = (*this) * a;
}

__forceinline Mat3 Mat3::operator*(const float a) const
{
	return Mat3(
		m[0] * a, m[3] * a, m[6] * a,
		m[1] * a, m[4] * a, m[7] * a,
		m[2] * a, m[5] * a, m[8] * a );
}

__forceinline Mat3 &Mat3::operator*=(const float a)
{
	return *this = (*this) * a;
}

__forceinline Mat3 Mat3::operator/(const float a) const
{
	float aInv = 1.0f / a;
	return Mat3(
		m[0] * aInv, m[3] * aInv, m[6] * aInv,
		m[1] * aInv, m[4] * aInv, m[7] * aInv,
		m[2] * aInv, m[5] * aInv, m[8] * aInv );
}

__forceinline Mat3 &Mat3::operator/=(const float a)
{
	return *this = (*this) / a;
}

__forceinline Vec4 Mat3::operator*(const Vec4 &a) const
{
	return Vec4(
		m[0] * a[0] + m[3] * a[1] + m[6] * a[2],
		m[1] * a[0] + m[4] * a[1] + m[7] * a[2],
		m[2] * a[0] + m[5] * a[1] + m[8] * a[2],
		a[3] );
}

__forceinline Vec3 Mat3::operator*(const Vec3 &a) const
{
	return Vec3(
		m[0] * a[0] + m[3] * a[1] + m[6] * a[2],
		m[1] * a[0] + m[4] * a[1] + m[7] * a[2],
		m[2] * a[0] + m[5] * a[1] + m[8] * a[2] );
}

__forceinline Mat3 operator*(const float f, const Mat3 &m)
{
	return m * f;
}

__forceinline Vec4 operator*(const Vec4 &v, const Mat3 &m)
{
	return m * v;
}

__forceinline Vec3 operator*(const Vec3 &v, const Mat3 &m)
{
	return m * v;
}

__forceinline Vec4 &operator*=(Vec4 &v, const Mat3 &m)
{
	v = m * v;
	return v;
}

__forceinline Vec3 &operator*=(Vec3 &v, const Mat3 &m)
{
	v = m * v;
	return v;
}

__forceinline bool Mat3::Compare(const Mat3 &a) const
{
	for (int i = 0; i < 9; i++)
	{
		if (this->m[i] != a.m[i])
		{
			return false;
		}
	}
	return true;
}

__forceinline bool Mat3::Compare(const Mat3 &a, const float epsilon ) const
{
	for (int i = 0; i < 9; i++)
	{
		if ( abs(this->m[i] - a.m[i]) > epsilon )
		{
			return false;
		}
	}
	return true;
}

__forceinline bool Mat3::operator==(const Mat3 &a) const
{
	return Compare(a);
}

__forceinline bool Mat3::operator!=(const Mat3 &a) const
{
	return !Compare(a);
}

__forceinline Vec3 Mat3::GetRow(int row) const
{
	assert(row >= 0 && row < 3);

	switch(row)
	{
	case 0:
		return Vec3(m[0], m[4], m[8]);

	case 1:
		return Vec3(m[1], m[5], m[9]);

	case 2:
		return Vec3(m[2], m[6], m[10]);
	}
}

__forceinline Vec3 Mat3::GetColumn(int column) const
{
	assert(column >= 0 && column < 3);

	switch(column)
	{
	case 0:
		return Vec3(m[0], m[1], m[2]);

	case 1:
		return Vec3(m[4], m[5], m[6]);

	case 2:
		return Vec3(m[8], m[9], m[10]);
	}
}

__forceinline void Mat3::Zero(void)
{
	memset( m, 0, sizeof(Mat3) );
}

__forceinline void Mat3::Identity()
{
	*this = mat3_identity;
}

__forceinline bool Mat3::IsIdentity(const float epsilon) const
{
	return Compare(mat3_identity, epsilon);
}

//====================================================================================================
// Mat4
//====================================================================================================

class Mat4
{
public:

	float m[16];

	Mat4() { this->Identity(); };

	explicit Mat4(	const float f0,  const float f1,  const float f2,  const float f3,
					const float f4,  const float f5,  const float f6,  const float f7,
					const float f8,  const float f9,  const float f10, const float f11,
					const float f12, const float f13, const float f14, const float f15 );
	explicit Mat4(const Mat3 &rotation, const Vec3 &translation );
	explicit Mat4(const float src[16]);

	operator float *();
	operator const float *() const;

	Mat4 &	operator=(Mat4 &a);
	Mat4	operator+(const Mat4 &a) const;
	Mat4 &	operator+=(const Mat4 &a);
	Mat4	operator-(const Mat4 &a) const;
	Mat4 &	operator-=(const Mat4 &a);
	Mat4	operator*(const Mat4 &a) const;
	Mat4 &	operator*=(const Mat4 &a);

	Mat4	operator*(const float a) const;
	Mat4 &	operator*=(const float a);
	Mat4	operator/(const float a) const;
	Mat4 &	operator/=(const float a);

	Vec4	operator*(const Vec4 &a) const;
	Vec3	operator*(const Vec3 &a) const;

	friend Mat4		operator*(const float f, const Mat4 &m);
	friend Vec4		operator*(const Vec4 &v, const Mat4 &m);
	friend Vec3		operator*(const Vec3 &v, const Mat4 &m);
	friend Vec4 &	operator*=(Vec4 &v, const Mat4 &m);
	friend Vec3 &	operator*=(Vec3 &v, const Mat4 &m);
	
	bool Compare(const Mat4 &a) const;
	bool Compare(const Mat4 &a, const float epsilon) const;
	bool operator==(const Mat4 &a) const;
	bool operator!=(const Mat4 &a) const;

	Vec4	GetRow(int row) const;
	Vec4	GetColumn(int column) const;

	void	Zero();
	void	Identity();
	bool	IsIdentity(const float epsilon = MATRIX_EPSILON) const;
    Mat4	Transpose() const;
	void	TransposeSelf();
	float	Determinant() const;
	Mat4	Inverse() const;
	void	InverseSelf();
	void	Scale(const float x, const float y, const float z);
	void	Translate(const Vec3 &v);

	void	BuildRotate(const Vec3 &axis, const float angle);
	void	BuildRotate(const float x, const float y, const float z, const float angle);
	void	BuildRotateX(const float angle);
	void	BuildRotateY(const float angle);
	void	BuildRotateZ(const float angle);
	void	Rotate(const Vec3 &axis, const float angle);
	void	Rotate(const float x, const float y, const float z, const float angle);
	void	RotateX(const float angle);
	void	RotateY(const float angle);
	void	RotateZ(const float angle);

	void	BuildProjection(float fov, float aspect, float zNear, float zFar);
	void	BuildViewMatrix(const Vec3 &eye, const Vec3 &center, const Vec3 &up);
};

extern Mat4 mat4_zero;
extern Mat4 mat4_identity;

__forceinline Mat4::Mat4(	const float f0,  const float f1,  const float f2,  const float f3,
							const float f4,  const float f5,  const float f6,  const float f7,
							const float f8,  const float f9,  const float f10, const float f11,
							const float f12, const float f13, const float f14, const float f15 )
{	
	m[0] = f0;	m[4] = f4;	m[ 8] =  f8; m[12] = f12;
	m[1] = f1;	m[5] = f5;	m[ 9] =  f9; m[13] = f13;
	m[2] = f2;	m[6] = f6;	m[10] = f10; m[14] = f14;
	m[3] = f3;	m[7] = f7;	m[11] = f11; m[15] = f15;
};

__forceinline Mat4::Mat4(const Mat3 &rotation, const Vec3 &translation )
{
	m[0] = rotation[0];	m[4] = rotation[3];	m[ 8] = rotation[6]; m[12] = translation[0];
	m[1] = rotation[1];	m[5] = rotation[4];	m[ 9] = rotation[7]; m[13] = translation[1];
	m[2] = rotation[2];	m[6] = rotation[5];	m[10] = rotation[8]; m[14] = translation[2];
	m[3] = 0.0f;		m[7] = 0.0f;		m[11] = 0.0f;		 m[15] = 1.0f;
}

__forceinline Mat4::Mat4(const float src[16])
{
	memcpy(m, src, 16 * sizeof(float));
};

__forceinline Mat4::operator float *()
{
	return m;
};
	
__forceinline Mat4::operator const float *() const
{
	return m;
};

__forceinline Mat4 &Mat4::operator=(Mat4 &a)
{
	memcpy(m, a.m, sizeof(float) * 16);
	return *this;
};

__forceinline Mat4 Mat4::operator+(const Mat4 &a) const
{
	return Mat4(
		m[0] + a.m[0], m[4] + a.m[4], m[ 8] + a.m[ 8], m[12] + a.m[12],
		m[1] + a.m[1], m[5] + a.m[5], m[ 9] + a.m[ 9], m[13] + a.m[13],
		m[2] + a.m[2], m[6] + a.m[6], m[10] + a.m[10], m[14] + a.m[14],
		m[3] + a.m[3], m[7] + a.m[7], m[11] + a.m[11], m[15] + a.m[15] );
}

__forceinline Mat4 &Mat4::operator+=(const Mat4 &a)
{
	return *this = (*this) + a;
}	

__forceinline Mat4 Mat4::operator-(const Mat4 &a) const
{
	return Mat4(
		m[0] - a.m[0], m[4] - a.m[4], m[ 8] - a.m[ 8], m[12] - a.m[12],
		m[1] - a.m[1], m[5] - a.m[5], m[ 9] - a.m[ 9], m[13] - a.m[13],
		m[2] - a.m[2], m[6] - a.m[6], m[10] - a.m[10], m[14] - a.m[14],
		m[3] - a.m[3], m[7] - a.m[7], m[11] - a.m[11], m[15] - a.m[15] );
}

__forceinline Mat4 &Mat4::operator-=(const Mat4 &a)
{
	return *this = (*this) - a;
}

__forceinline Mat4 Mat4::operator*(const Mat4 &a) const
{
	return Mat4(
		m[0] * a[ 0] + m[4] * a[ 1] + m[ 8] * a[ 2] + m[12] * a[ 3],
		m[1] * a[ 0] + m[5] * a[ 1] + m[ 9] * a[ 2] + m[13] * a[ 3],
		m[2] * a[ 0] + m[6] * a[ 1] + m[10] * a[ 2] + m[14] * a[ 3],
		m[3] * a[ 0] + m[7] * a[ 1] + m[11] * a[ 2] + m[15] * a[ 3],
		m[0] * a[ 4] + m[4] * a[ 5] + m[ 8] * a[ 6] + m[12] * a[ 7],
		m[1] * a[ 4] + m[5] * a[ 5] + m[ 9] * a[ 6] + m[13] * a[ 7],
		m[2] * a[ 4] + m[6] * a[ 5] + m[10] * a[ 6] + m[14] * a[ 7],
		m[3] * a[ 4] + m[7] * a[ 5] + m[11] * a[ 6] + m[15] * a[ 7],
		m[0] * a[ 8] + m[4] * a[ 9] + m[ 8] * a[10] + m[12] * a[11],
		m[1] * a[ 8] + m[5] * a[ 9] + m[ 9] * a[10] + m[13] * a[11],
		m[2] * a[ 8] + m[6] * a[ 9] + m[10] * a[10] + m[14] * a[11],
		m[3] * a[ 8] + m[7] * a[ 9] + m[11] * a[10] + m[15] * a[11],
		m[0] * a[12] + m[4] * a[13] + m[ 8] * a[14] + m[12] * a[15],
		m[1] * a[12] + m[5] * a[13] + m[ 9] * a[14] + m[13] * a[15],
		m[2] * a[12] + m[6] * a[13] + m[10] * a[14] + m[14] * a[15],
		m[3] * a[12] + m[7] * a[13] + m[11] * a[14] + m[15] * a[15] );
}

__forceinline Mat4 &Mat4::operator*=(const Mat4 &a)
{
	return *this = (*this) * a;
}

__forceinline Mat4 Mat4::operator*(const float a) const
{
	return Mat4(
		m[0] * a, m[4] * a, m[ 8] * a, m[12] * a,
		m[1] * a, m[5] * a, m[ 9] * a, m[13] * a,
		m[2] * a, m[6] * a, m[10] * a, m[14] * a,
		m[3] * a, m[7] * a, m[11] * a, m[15] * a );
}

__forceinline Mat4 &Mat4::operator*=(const float a)
{
	return *this = (*this) * a;
}

__forceinline Mat4 Mat4::operator/(const float a) const
{
	float aInv = 1.0f / a;
	return Mat4(
		m[0] * aInv, m[4] * aInv, m[ 8] * aInv, m[12] * aInv,
		m[1] * aInv, m[5] * aInv, m[ 9] * aInv, m[13] * aInv,
		m[2] * aInv, m[6] * aInv, m[10] * aInv, m[14] * aInv,
		m[3] * aInv, m[7] * aInv, m[11] * aInv, m[15] * aInv );
}

__forceinline Mat4 &Mat4::operator/=(const float a)
{
	return *this = (*this) / a;
}

__forceinline Vec4 Mat4::operator*(const Vec4 &a) const
{
	return Vec4(
		m[0] * a[0] + m[4] * a[1] + m[ 8] * a[2] + m[12] * a[3],
		m[1] * a[0] + m[5] * a[1] + m[ 9] * a[2] + m[13] * a[3],
		m[2] * a[0] + m[6] * a[1] + m[10] * a[2] + m[14] * a[3],
		m[3] * a[0] + m[7] * a[1] + m[11] * a[2] + m[15] * a[3] );
}

__forceinline Vec3 Mat4::operator*(const Vec3 &a) const
{
	return Vec3(
		m[0] * a[0] + m[4] * a[1] + m[ 8] * a[2] + m[12],
		m[1] * a[0] + m[5] * a[1] + m[ 9] * a[2] + m[13],
		m[2] * a[0] + m[6] * a[1] + m[10] * a[2] + m[14] );
}

__forceinline Mat4 operator*(const float f, const Mat4 &m)
{
	return m * f;
}

__forceinline Vec4 operator*(const Vec4 &v, const Mat4 &m)
{
	return m * v;
}

__forceinline Vec3 operator*(const Vec3 &v, const Mat4 &m)
{
	return m * v;
}

__forceinline Vec4 &operator*=(Vec4 &v, const Mat4 &m)
{
	v = m * v;
	return v;
}

__forceinline Vec3 &operator*=(Vec3 &v, const Mat4 &m)
{
	v = m * v;
	return v;
}

__forceinline bool Mat4::Compare(const Mat4 &a) const
{
	for (int i = 0; i < 16; i++)
	{
		if (this->m[i] != a.m[i])
		{
			return false;
		}
	}
	return true;
}

__forceinline bool Mat4::Compare(const Mat4 &a, const float epsilon ) const
{
	for (int i = 0; i < 16; i++)
	{
		if ( abs(this->m[i] - a.m[i]) > epsilon )
		{
			return false;
		}
	}
	return true;
}

__forceinline bool Mat4::operator==(const Mat4 &a) const
{
	return Compare(a);
}

__forceinline bool Mat4::operator!=(const Mat4 &a) const
{
	return !Compare(a);
}

__forceinline Vec4 Mat4::GetRow(int row) const
{
	assert(row >= 0 && row < 4);

	switch(row)
	{
	case 0:
		return Vec4(m[0], m[4], m[8], m[12]);

	case 1:
		return Vec4(m[1], m[5], m[9], m[13]);

	case 2:
		return Vec4(m[2], m[6], m[10], m[14]);

	case 3:
		return Vec4(m[3], m[7], m[11], m[15]);
	}
}

__forceinline Vec4 Mat4::GetColumn(int column) const
{
	assert(column >= 0 && column < 4);

	switch(column)
	{
	case 0:
		return Vec4(m[0], m[1], m[2], m[3]);

	case 1:
		return Vec4(m[4], m[5], m[6], m[7]);

	case 2:
		return Vec4(m[8], m[9], m[10], m[11]);

	case 3:
		return Vec4(m[12], m[13], m[14], m[15]);
	}
}

__forceinline void Mat4::Zero(void)
{
	memset( m, 0, sizeof(Mat4) );
}

__forceinline void Mat4::Identity()
{
	*this = mat4_identity;
}

__forceinline bool Mat4::IsIdentity(const float epsilon) const
{
	return Compare(mat4_identity, epsilon);
}

#endif /*__MATRIX_H__*/