
#include <windows.h>
#include <math.h>

#include "Frustum.h"

void Frustum::ComputeFrustum(const Mat4 &mProjection, const Mat4 &mModelView)
{
	Mat4 mClip;

	mClip = mProjection * mModelView;
	
	// Extrair os planos que limitam a cena

	m_Frustum[RIGHT].a = mClip.m[ 3] - mClip.m[ 0];
	m_Frustum[RIGHT].b = mClip.m[ 7] - mClip.m[ 4];
	m_Frustum[RIGHT].c = mClip.m[11] - mClip.m[ 8];
	m_Frustum[RIGHT].d = mClip.m[15] - mClip.m[12];
	m_Frustum[RIGHT].Normalize();

	m_Frustum[LEFT].a = mClip.m[ 3] + mClip.m[ 0];
	m_Frustum[LEFT].b = mClip.m[ 7] + mClip.m[ 4];
	m_Frustum[LEFT].c = mClip.m[11] + mClip.m[ 8];
	m_Frustum[LEFT].d = mClip.m[15] + mClip.m[12];
	m_Frustum[LEFT].Normalize();

	m_Frustum[BOTTOM].a = mClip.m[ 3] + mClip.m[ 1];
	m_Frustum[BOTTOM].b = mClip.m[ 7] + mClip.m[ 5];
	m_Frustum[BOTTOM].c = mClip.m[11] + mClip.m[ 9];
	m_Frustum[BOTTOM].d = mClip.m[15] + mClip.m[13];
	m_Frustum[BOTTOM].Normalize();

	m_Frustum[TOP].a = mClip.m[ 3] - mClip.m[ 1];
	m_Frustum[TOP].b = mClip.m[ 7] - mClip.m[ 5];
	m_Frustum[TOP].c = mClip.m[11] - mClip.m[ 9];
	m_Frustum[TOP].d = mClip.m[15] - mClip.m[13];
	m_Frustum[TOP].Normalize();

	m_Frustum[BACK].a = mClip.m[ 3] - mClip.m[ 2];
	m_Frustum[BACK].b = mClip.m[ 7] - mClip.m[ 6];
	m_Frustum[BACK].c = mClip.m[11] - mClip.m[10];
	m_Frustum[BACK].d = mClip.m[15] - mClip.m[14];
	m_Frustum[BACK].Normalize();

	m_Frustum[FRONT].a = mClip.m[ 3] + mClip.m[ 2];
	m_Frustum[FRONT].b = mClip.m[ 7] + mClip.m[ 6];
	m_Frustum[FRONT].c = mClip.m[11] + mClip.m[10];
	m_Frustum[FRONT].d = mClip.m[15] + mClip.m[14];
	m_Frustum[FRONT].Normalize();
}

float Frustum::DistToEye(const Vec3 &pos) const
{
	return m_Frustum[FRONT].a * pos.x + m_Frustum[FRONT].b * pos.y + m_Frustum[FRONT].c * pos.z + m_Frustum[FRONT].d;
}

bool Frustum::PointInFrustum(const Vec3 &pos) const
{
	for(int i = 0; i < 6; i++)
		if(m_Frustum[i].a * pos.x + m_Frustum[i].b * pos.y + m_Frustum[i].c * pos.z + m_Frustum[i].d <= 0)
			return false;

	return true;
}

float Frustum::SphereInFrustum(const Vec3 &pos, float radius) const
{
	float d;

	for(int i = 0; i < 6; i++ )	
		if( d = m_Frustum[i].a * pos.x + m_Frustum[i].b * pos.y + m_Frustum[i].c * pos.z + m_Frustum[i].d )
		if( d <= -radius)
			return 0.0f;

	return d + radius;
}

bool Frustum::CubeInFrustum(const Vec3 &pos, float size) const
{
	for(int i = 0; i < 6; i++)
	{
		if(m_Frustum[i].a * (pos.x + size) + m_Frustum[i].b * (pos.y + size) + m_Frustum[i].c * (pos.z + size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x + size) + m_Frustum[i].b * (pos.y + size) + m_Frustum[i].c * (pos.z - size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x + size) + m_Frustum[i].b * (pos.y - size) + m_Frustum[i].c * (pos.z + size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x + size) + m_Frustum[i].b * (pos.y - size) + m_Frustum[i].c * (pos.z - size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - size) + m_Frustum[i].b * (pos.y + size) + m_Frustum[i].c * (pos.z + size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - size) + m_Frustum[i].b * (pos.y + size) + m_Frustum[i].c * (pos.z - size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - size) + m_Frustum[i].b * (pos.y - size) + m_Frustum[i].c * (pos.z + size) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - size) + m_Frustum[i].b * (pos.y - size) + m_Frustum[i].c * (pos.z - size) + m_Frustum[i].d > 0)
		   continue;

		return false;
	}

	return true;
}

bool Frustum::ParallelepipedInFrustum(const Vec3 &pos, float width, float height, float depth) const
{
	width	*= 0.5;
	height	*= 0.5;
	depth	*= 0.5;
	
	for(int i = 0; i < 6; i++)
	{
		if(m_Frustum[i].a * (pos.x + width) + m_Frustum[i].b * (pos.y + height) + m_Frustum[i].c * (pos.z + depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x + width) + m_Frustum[i].b * (pos.y + height) + m_Frustum[i].c * (pos.z - depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x + width) + m_Frustum[i].b * (pos.y - height) + m_Frustum[i].c * (pos.z + depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x + width) + m_Frustum[i].b * (pos.y - height) + m_Frustum[i].c * (pos.z - depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - width) + m_Frustum[i].b * (pos.y + height) + m_Frustum[i].c * (pos.z + depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - width) + m_Frustum[i].b * (pos.y + height) + m_Frustum[i].c * (pos.z - depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - width) + m_Frustum[i].b * (pos.y - height) + m_Frustum[i].c * (pos.z + depth) + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * (pos.x - width) + m_Frustum[i].b * (pos.y - height) + m_Frustum[i].c * (pos.z - depth) + m_Frustum[i].d > 0)
		   continue;

		return false;
	}

	return true;
}


bool Frustum::ParallelepipedInFrustum(const Vec3 &pos, const Vec3 *v) const
{
	Vec3 v1 = v[0] + pos;
	Vec3 v2 = v[1] + pos;
	Vec3 v3 = v[2] + pos;
	Vec3 v4 = v[3] + pos;
	Vec3 v5 = v[4] + pos;
	Vec3 v6 = v[5] + pos;
	Vec3 v7 = v[6] + pos;
	Vec3 v8 = v[7] + pos;

	for(int i = 0; i < 6; i++)
	{
		if(m_Frustum[i].a * v1.x + m_Frustum[i].b * v1.y + m_Frustum[i].c * v1.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v2.x + m_Frustum[i].b * v2.y + m_Frustum[i].c * v2.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v3.x + m_Frustum[i].b * v3.y + m_Frustum[i].c * v3.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v4.x + m_Frustum[i].b * v4.y + m_Frustum[i].c * v4.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v5.x + m_Frustum[i].b * v5.y + m_Frustum[i].c * v5.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v6.x + m_Frustum[i].b * v6.y + m_Frustum[i].c * v6.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v7.x + m_Frustum[i].b * v7.y + m_Frustum[i].c * v7.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v8.x + m_Frustum[i].b * v8.y + m_Frustum[i].c * v8.z + m_Frustum[i].d > 0)
		   continue;

		return false;
	}

	return true;
}

bool Frustum::ParallelepipedInFrustum(const Mat4 &mPose, const Vec3 *v) const
{
	Vec4 v1 = mPose * Vec4(v[0], 1.0f);
	Vec4 v2 = mPose * Vec4(v[1], 1.0f);
	Vec4 v3 = mPose * Vec4(v[2], 1.0f);
	Vec4 v4 = mPose * Vec4(v[3], 1.0f);
	Vec4 v5 = mPose * Vec4(v[4], 1.0f);
	Vec4 v6 = mPose * Vec4(v[5], 1.0f);
	Vec4 v7 = mPose * Vec4(v[6], 1.0f);
	Vec4 v8 = mPose * Vec4(v[7], 1.0f);

	for(int i = 0; i < 6; i++)
	{
		if(m_Frustum[i].a * v1.x + m_Frustum[i].b * v1.y + m_Frustum[i].c * v1.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v2.x + m_Frustum[i].b * v2.y + m_Frustum[i].c * v2.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v3.x + m_Frustum[i].b * v3.y + m_Frustum[i].c * v3.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v4.x + m_Frustum[i].b * v4.y + m_Frustum[i].c * v4.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v5.x + m_Frustum[i].b * v5.y + m_Frustum[i].c * v5.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v6.x + m_Frustum[i].b * v6.y + m_Frustum[i].c * v6.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v7.x + m_Frustum[i].b * v7.y + m_Frustum[i].c * v7.z + m_Frustum[i].d > 0)
		   continue;
		if(m_Frustum[i].a * v8.x + m_Frustum[i].b * v8.y + m_Frustum[i].c * v8.z + m_Frustum[i].d > 0)
		   continue;

		return false;
	}

	return true;
}