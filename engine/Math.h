
#ifndef __MATH_H__
#define __MATH_H__

#define PI		3.14159265358979323846f
#define TWO_PI	6.28318530717958647692f

#define DEG2RAD(a)	( (a) * PI / 180.0f )
#define RAD2DEG(a)	( (a) * 180.0f / PI )

template <class T>
__forceinline T clamp(T x, T min, T max)
{ 
	return (x < min) ? min : (x > max) ? max : x;
}

#endif /*__MATH_H__*/