
#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <assert.h>
#include <math.h>

#include "Math.h"

class Vec2;
class Vec3;
class Vec4;

//====================================================================================================
// Vec2
//====================================================================================================

class Vec2
{
public:

	float x, y;
	
	Vec2() {};
	explicit Vec2(const float x, const float y);
	explicit Vec2(const float a);
	
	void Set(const float x, const float y);	
	void Zero();

	operator float *();
	operator const float *() const;

	float	operator[](const int i) const;
	float &	operator[](const int i);

	Vec2	operator+() const;
    Vec2	operator-() const;

	Vec2 &	operator=(const Vec2 &a);
	Vec2	operator+(const Vec2 &a) const;
	Vec2 &	operator+=(const Vec2 &a);
	Vec2	operator-(const Vec2 &a) const;
	Vec2 &	operator-=(const Vec2 &a);
	Vec2 	operator*(const Vec2 &a) const;
    Vec2 &	operator*=(const Vec2 &a);  
	Vec2 	operator/(const Vec2 &a) const;
    Vec2 &	operator/=(const Vec2 &a);

	Vec2	operator*(const float a) const;
    Vec2 &	operator*=(const float a);
	Vec2	operator/(const float a) const;
	Vec2 &	operator/=(const float a);

	friend Vec2 operator*(const float a, const Vec2 b);

	bool Compare(const Vec2 &a) const;
	bool Compare(const Vec2 &a, const float epsilon) const;
	bool operator==(const Vec2 &a) const;
	bool operator!=(const Vec2 &a) const;

	float	Length() const;
	float	Normalize();
	void	Clamp(const Vec2 &min, const Vec2 &max);
};

__forceinline Vec2::Vec2(const float x, const float y)
{
	this->x = x;
	this->y = y;
};

__forceinline Vec2::Vec2(const float a)
{
	this->x = a;
	this->y = a;
};

__forceinline void Vec2::Set(const float x, const float y)
{
	this->x = x;
	this->y = y;
};

__forceinline void Vec2::Zero()
{
	x = y = 0.0f;
}

__forceinline Vec2::operator float *()
{
	return &x;
};

__forceinline Vec2::operator const float *() const
{
	return &x;
};

__forceinline float Vec2::operator[](const int i) const
{
	assert( (i >= 0) && (i < 2) );
	return (&x)[i];
}

__forceinline float &Vec2::operator[](const int i)
{
	assert( (i >= 0) && (i < 2) );
	return (&x)[i];
}

__forceinline Vec2 Vec2::operator+() const
{
	return *this;
};

__forceinline Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
};

__forceinline Vec2 &Vec2::operator=(const Vec2 &a)
{
	x = a.x;
	y = a.y;

	return *this;
};

__forceinline Vec2 Vec2::operator+(const Vec2 &a) const
{
	return Vec2(x + a.x, y + a.y);
};

__forceinline Vec2 &Vec2::operator+=(const Vec2 &a)
{
	x += a.x;
	y += a.y;

	return *this;
};

__forceinline Vec2	Vec2::operator-(const Vec2 &a) const
{
	return Vec2(x - a.x, y - a.y);
};

__forceinline Vec2 &Vec2::operator-=(const Vec2 &a)
{
	x -= a.x;
	y -= a.y;
	
	return *this;
};

__forceinline Vec2 Vec2::operator*(const Vec2 &a) const
{
	return Vec2(x * a.x, y * a.y);
}

__forceinline Vec2 &Vec2::operator*=(const Vec2 &a)
{
	x *= a.x;
	y *= a.y;

	return *this;
}

__forceinline Vec2 Vec2::operator/(const Vec2 &a) const
{
	return Vec2(x / a.x, y / a.y);
}

__forceinline Vec2 &Vec2::operator/=(const Vec2 &a)
{
	x /= a.x;
	y /= a.y;

	return *this;
}

__forceinline Vec2	Vec2::operator*(const float a) const
{
	return Vec2(x * a, y * a);
};
    
__forceinline Vec2 &Vec2::operator*=(const float a)
{
	x *= a;
	y *= a;

	return *this;
};

__forceinline Vec2 Vec2::operator/(const float a) const
{
	assert(a != 0.0f);
	float aInv = 1.0f / a;
	return Vec2(x * aInv, y * aInv);
};

__forceinline Vec2 &Vec2::operator/=(const float a)
{
	assert(a != 0.0f);
	float aInv = 1.0f /a;
    x *= aInv;
	y *= aInv;

	return *this;
}

__forceinline Vec2 operator*(const float a, const Vec2 b)
{
	return Vec2(b.x * a, b.y * a);
}

__forceinline bool Vec2::Compare(const Vec2 &a) const
{
	return ( (x == a.x) && (y == a.y) );
}

__forceinline bool Vec2::Compare(const Vec2 &a, const float epsilon) const
{
	if( abs(x - a.x) > epsilon )
		return false;
			
	if( abs(y - a.y) > epsilon )
		return false;

	return true;
}

__forceinline bool Vec2::operator==(const Vec2 &a) const
{
	return Compare(a);
}

__forceinline bool Vec2::operator!=(const Vec2 &a) const
{
	return !Compare(a);
}

__forceinline float Vec2::Length() const
{
	return sqrtf(x*x + y*y);
}

__forceinline float Vec2::Normalize()
{
	float length = Length();

	if(length != 0)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
	}
	else
	{
        x = y = 0.0f;
	}

	return length;
}

__forceinline void Vec2::Clamp(const Vec2 &min, const Vec2 &max)
{
	if(x < min.x)
		x = min.x;
	else if(x > max.x)
		x = max.x;

	if(y < min.y)
		y = min.y;
	else if(y > max.y)
		y = max.y;
}

//====================================================================================================
// Vec3
//====================================================================================================

class Vec3
{
public:

	float x, y, z;
	
	Vec3() {};
	explicit Vec3(const float x, const float y, const float z);
	explicit Vec3(const float a);
	explicit Vec3(const Vec4 &v);
	
	void Set(const float x, const float y, const float z);	
	void Zero();

	operator float *();
	operator const float *() const;

	float	operator[](const int i) const;
	float &	operator[](const int i);

	Vec3	operator+() const;
    Vec3	operator-() const;

	Vec3 &	operator=(const Vec3 &a);
	Vec3	operator+(const Vec3 &a) const;
	Vec3 &	operator+=(const Vec3 &a);
	Vec3	operator-(const Vec3 &a) const;
	Vec3 &	operator-=(const Vec3 &a);
	Vec3 	operator*(const Vec3 &a) const;
    Vec3 &	operator*=(const Vec3 &a);  
	Vec3 	operator/(const Vec3 &a) const;
    Vec3 &	operator/=(const Vec3 &a);

	Vec3	operator*(const float a) const;
    Vec3 &	operator*=(const float a);
	Vec3	operator/(const float a) const;
	Vec3 &	operator/=(const float a);

	friend Vec3 operator*(const float a, const Vec3 b);

	bool Compare(const Vec3 &a) const;
	bool Compare(const Vec3 &a, const float epsilon) const;
	bool operator==(const Vec3 &a) const;
	bool operator!=(const Vec3 &a) const;

	float	Dot(const Vec3 &a) const;
	Vec3	Cross(const Vec3 &a) const;
	Vec3 &	Cross(const Vec3 &a, const Vec3 &b);
	float	Length() const;
	float	Normalize();
	void	Clamp(const Vec3 &min, const Vec3 &max);
};

__forceinline Vec3::Vec3(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
};

__forceinline Vec3::Vec3(const float a)
{
	this->x = a;
	this->y = a;
	this->z = a;
};

__forceinline void Vec3::Set(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
};

__forceinline void Vec3::Zero()
{
	x = y = z = 0.0f;
}

__forceinline Vec3::operator float *()
{
	return &x;
};

__forceinline Vec3::operator const float *() const
{
	return &x;
};

__forceinline float Vec3::operator[](const int i) const
{
	assert( (i >= 0) && (i < 3) );
	return (&x)[i];
}

__forceinline float &Vec3::operator[](const int i)
{
	assert( (i >= 0) && (i < 3) );
	return (&x)[i];
}

__forceinline Vec3 Vec3::operator+() const
{
	return *this;
};

__forceinline Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
};

__forceinline Vec3 &Vec3::operator=(const Vec3 &a)
{
	x  = a.x;
	y  = a.y;
	z  = a.z;

	return *this;
};

__forceinline Vec3 Vec3::operator+(const Vec3 &a) const
{
	return Vec3(x + a.x, y + a.y, z + a.z);
};

__forceinline Vec3 &Vec3::operator+=(const Vec3 &a)
{
	x += a.x;
	y += a.y;
	z += a.z;

	return *this;
};

__forceinline Vec3	Vec3::operator-(const Vec3 &a) const
{
	return Vec3(x - a.x, y - a.y, z - a.z);
};

__forceinline Vec3 &Vec3::operator-=(const Vec3 &a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	
	return *this;
};

__forceinline Vec3 Vec3::operator*(const Vec3 &a) const
{
	return Vec3(x * a.x, y * a.y, z * a.z);
}

__forceinline Vec3 &Vec3::operator*=(const Vec3 &a)
{
	x *= a.x;
	y *= a.y;
	z *= a.z;

	return *this;
}

__forceinline Vec3 Vec3::operator/(const Vec3 &a) const
{
	return Vec3(x / a.x, y / a.y, z / a.z);
}

__forceinline Vec3 &Vec3::operator/=(const Vec3 &a)
{
	x /= a.x;
	y /= a.y;
	z /= a.z;

	return *this;
}

__forceinline Vec3	Vec3::operator*(const float a) const
{
	return Vec3(x * a, y * a, z * a);
};
    
__forceinline Vec3 &Vec3::operator*=(const float a)
{
	x *= a;
	y *= a;
	z *= a;

	return *this;
};

__forceinline Vec3 Vec3::operator/(const float a) const
{
	assert(a != 0.0f);
	float aInv = 1.0f / a;
	return Vec3(x * aInv, y * aInv, z * aInv);
};

__forceinline Vec3 &Vec3::operator/=(const float a)
{
	assert(a != 0.0f);
	float aInv = 1.0f /a;
    x *= aInv;
	y *= aInv;
	z *= aInv;

	return *this;
}

__forceinline Vec3 operator*(const float a, const Vec3 b)
{
	return Vec3(b.x * a, b.y * a, b.z * a);
}

__forceinline bool Vec3::Compare(const Vec3 &a) const
{
	return ( (x == a.x) && (y == a.y) && (z == a.z) );
}

__forceinline bool Vec3::Compare(const Vec3 &a, const float epsilon) const
{
	if( abs(x - a.x) > epsilon )
		return false;
			
	if( abs(y - a.y) > epsilon )
		return false;

	if( abs(z - a.z) > epsilon )
		return false;

	return true;
}

__forceinline bool Vec3::operator==(const Vec3 &a) const
{
	return Compare(a);
}

__forceinline bool Vec3::operator!=(const Vec3 &a) const
{
	return !Compare(a);
}

__forceinline float Vec3::Dot(const Vec3 &a) const
{
	return this->x * a.x + this->y * a.y + this->z * a.z;
}

__forceinline Vec3 Vec3::Cross(const Vec3 &a) const
{
	return Vec3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
}

__forceinline Vec3 &Vec3::Cross(const Vec3 &a, const Vec3 &b)
{
	x = a.y * b.z - a.z * b.y;
	y = a.z * b.x - a.x * b.z;
	z = a.x * b.y - a.y * b.x;

	return *this;
}

__forceinline float Vec3::Length() const
{
	return sqrtf(x*x + y*y + z*z);
}

__forceinline float Vec3::Normalize()
{
	float length = Length();

	if(length != 0)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
	else
	{
        x = y = z = 0.0f;
	}

	return length;
}

__forceinline void Vec3::Clamp(const Vec3 &min, const Vec3 &max)
{
	if(x < min.x)
		x = min.x;
	else if(x > max.x)
		x = max.x;

	if(y < min.y)
		y = min.y;
	else if(y > max.y)
		y = max.y;

	if(z < min.z)
		z = min.z;
	else if(z > max.z)
		z = max.z;
}

//====================================================================================================
// Vec4
//====================================================================================================

class Vec4
{
public:

	float x, y, z, w;
	
	Vec4() {};
	explicit Vec4(const float x, const float y, const float z, const float w);
	explicit Vec4(const float a);
	explicit Vec4(const Vec3 &v, const float w);
	
	void Set(const float x, const float y, const float z, const float w);	
	void Zero();

	operator float *();
	operator const float *() const;

	float	operator[](const int i) const;
	float &	operator[](const int i);

	Vec4	operator+() const;
    Vec4	operator-() const;

	Vec4 &	operator=(const Vec4 &a);
	Vec4	operator+(const Vec4 &a) const;
	Vec4 &	operator+=(const Vec4 &a);
	Vec4	operator-(const Vec4 &a) const;
	Vec4 &	operator-=(const Vec4 &a);
	Vec4 	operator*(const Vec4 &a) const;
    Vec4 &	operator*=(const Vec4 &a);  
	Vec4 	operator/(const Vec4 &a) const;
    Vec4 &	operator/=(const Vec4 &a);

	Vec4	operator*(const float a) const;
    Vec4 &	operator*=(const float a);
	Vec4	operator/(const float a) const;
	Vec4 &	operator/=(const float a);

	friend Vec4 operator*(const float a, const Vec4 b);

	bool Compare(const Vec4 &a) const;
	bool Compare(const Vec4 &a, const float epsilon) const;
	bool operator==(const Vec4 &a) const;
	bool operator!=(const Vec4 &a) const;

	float	Length() const;
	float	Normalize();
	void	Clamp(const Vec4 &min, const Vec4 &max);
};

__forceinline Vec4::Vec4(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
};

__forceinline Vec4::Vec4(const float a)
{
	this->x = a;
	this->y = a;
	this->z = a;
	this->w = a;
};

__forceinline Vec4::Vec4(const Vec3 &v, const float w)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = w;
}

__forceinline void Vec4::Set(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
};

__forceinline void Vec4::Zero()
{
	x = y = z = w = 0.0f;
}

__forceinline Vec4::operator float *()
{
	return &x;
};

__forceinline Vec4::operator const float *() const
{
	return &x;
};

__forceinline float Vec4::operator[](const int i) const
{
	assert( (i >= 0) && (i < 4) );
	return (&x)[i];
}

__forceinline float &Vec4::operator[](const int i)
{
	assert( (i >= 0) && (i < 4) );
	return (&x)[i];
}

__forceinline Vec4 Vec4::operator+() const
{
	return *this;
};

__forceinline Vec4 Vec4::operator-() const
{
	return Vec4(-x, -y, -z, -w);
};

__forceinline Vec4 &Vec4::operator=(const Vec4 &a)
{
	x  = a.x;
	y  = a.y;
	z  = a.z;
	w  = a.w;

	return *this;
};

__forceinline Vec4 Vec4::operator+(const Vec4 &a) const
{
	return Vec4(x + a.x, y + a.y, z + a.z, w + a.w);
};

__forceinline Vec4 &Vec4::operator+=(const Vec4 &a)
{
	x += a.x;
	y += a.y;
	z += a.z;
	w += a.w;

	return *this;
};

__forceinline Vec4	Vec4::operator-(const Vec4 &a) const
{
	return Vec4(x - a.x, y - a.y, z - a.z, w - a.w);
};

__forceinline Vec4 &Vec4::operator-=(const Vec4 &a)
{
	x -= a.x;
	y -= a.y;
	z -= a.z;
	w -= a.w;
	
	return *this;
};

__forceinline Vec4 Vec4::operator*(const Vec4 &a) const
{
	return Vec4(x * a.x, y * a.y, z * a.z, w * a.w);
}

__forceinline Vec4 &Vec4::operator*=(const Vec4 &a)
{
	x *= a.x;
	y *= a.y;
	z *= a.z;
	w *= a.w;

	return *this;
}

__forceinline Vec4 Vec4::operator/(const Vec4 &a) const
{
	return Vec4(x / a.x, y / a.y, z / a.z, w / a.w);
}

__forceinline Vec4 &Vec4::operator/=(const Vec4 &a)
{
	x /= a.x;
	y /= a.y;
	z /= a.z;
	w /= a.w;

	return *this;
}

__forceinline Vec4	Vec4::operator*(const float a) const
{
	return Vec4(x * a, y * a, z * a, w * a);
};
    
__forceinline Vec4 &Vec4::operator*=(const float a)
{
	x *= a;
	y *= a;
	z *= a;
	w *= a;

	return *this;
};

__forceinline Vec4 Vec4::operator/(const float a) const
{
	assert(a != 0.0f);
	float aInv = 1.0f / a;
	return Vec4(x * aInv, y * aInv, z * aInv, w * aInv);
};

__forceinline Vec4 &Vec4::operator/=(const float a)
{
	assert(a != 0.0f);
	float aInv = 1.0f /a;
    x *= aInv;
	y *= aInv;
	z *= aInv;
	w *= aInv;

	return *this;
}

__forceinline Vec4 operator*(const float a, const Vec4 b)
{
	return Vec4(b.x * a, b.y * a, b.z * a, b.w * a);
}

__forceinline bool Vec4::Compare(const Vec4 &a) const
{
	return ( (x == a.x) && (y == a.y) && (z == a.z) && (w == a.w) );
}

__forceinline bool Vec4::Compare(const Vec4 &a, const float epsilon) const
{
	if( abs(x - a.x) > epsilon )
		return false;
			
	if( abs(y - a.y) > epsilon )
		return false;

	if( abs(z - a.z) > epsilon )
		return false;

	if( abs(w - a.w) > epsilon )
		return false;

	return true;
}

__forceinline bool Vec4::operator==(const Vec4 &a) const
{
	return Compare(a);
}

__forceinline bool Vec4::operator!=(const Vec4 &a) const
{
	return !Compare(a);
}

__forceinline float Vec4::Length() const
{
	return sqrtf(x*x + y*y + z*z + w*w);
}

__forceinline float Vec4::Normalize()
{
	float length = Length();

	if(length != 0)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
		w *= invLength;
	}
	else
	{
        x = y = z = w = 0.0f;
	}

	return length;
}

__forceinline void Vec4::Clamp(const Vec4 &min, const Vec4 &max)
{
	if(x < min.x)
		x = min.x;
	else if(x > max.x)
		x = max.x;

	if(y < min.y)
		y = min.y;
	else if(y > max.y)
		y = max.y;

	if(z < min.z)
		z = min.z;
	else if(z > max.z)
		z = max.z;

	if(w < min.w)
		w = min.w;
	else if(w > max.w)
		w = max.w;
}

__forceinline float Length(const Vec2 &v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

__forceinline Vec2 Normalize(const Vec2 &v)
{
	float length = v.Length();

	if(length != 0)
	{
		float invLength = 1.0f / length;
		return Vec2(v.x * invLength, v.y * invLength);
	}
	else
	{
        return Vec2(0.0f, 0.0f);
	}
}

__forceinline Vec3::Vec3(const Vec4 &v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

__forceinline float Dot(const Vec3 &a, const Vec3 &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

__forceinline Vec3 Cross(const Vec3 &a, const Vec3 &b)
{
	return Vec3(
		(a.y * b.z) - (a.z * b.y),
	    (a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x)
	);
}

__forceinline float Length(const Vec3 &v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

__forceinline Vec3 Normalize(const Vec3 &v)
{
	float length = v.Length();

	if(length != 0)
	{
		float invLength = 1.0f / length;
		return Vec3(v.x * invLength, v.y * invLength, v.z * invLength);
	}
	else
	{
        return Vec3(0.0f, 0.0f, 0.0f);
	}
}

#endif /*__VECTOR_H__*/