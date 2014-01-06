
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Vector.h"
#include "Matrix.h"

class Camera {

public:
	Camera();
	~Camera() {};



	void PositionCamera(float eyeX,		float eyeY,		float eyeZ,
			 		    float centerX,	float centerY,	float centerZ,
						float upX,		float upY,		float upZ);

    Vec3 GetAxisX() const;
	Vec3 GetAxisY() const;
	Vec3 GetAxisZ() const;

	void MoveForward(float units);
	void Strafe(float units);
	void RotateView(float angle, float x, float y, float z);

	void SetViewByMouse(float angleY, float angleZ);

	void BuildViewMatrix(Mat4 *m) const;

	void SetMouseRotSpeed(float speed);
	void SetMaxAngleX(float angle);

	Vec3 m_vEye;
	Vec3 m_vCenter;
	Vec3 m_vUp;
};

#endif