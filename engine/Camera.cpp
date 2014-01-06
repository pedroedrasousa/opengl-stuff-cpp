
#include <windows.h>
#include <stdio.h>
#include <math.h>	
#include <gl\gl.h>

#include "Camera.h"
#include "Vector.h"
#include "Matrix.h"

#include "OpenGLApp.h"

extern OpenGLApp *g_OpenGLApp;

Camera::Camera()
{
	m_vEye.Set(0.0, 0.0, 0.0);
	m_vCenter.Set(0.0, -1.0, 0.0);
	m_vUp.Set(0.0, 1.0, 0.0);
}

void Camera::PositionCamera(	float eyeX,		float eyeY,		float eyeZ,
					  			float centerX,	float centerY,	float centerZ,
								float upX,		float upY,		float upZ) 
{
	m_vEye.Set(eyeX, eyeY, eyeZ);
	m_vCenter.Set(centerX, centerY, centerZ);
	m_vUp.Set(upX, upY, upZ);
}

Vec3 Camera::GetAxisX() const
{
	return Normalize( Cross( m_vCenter - m_vEye, m_vUp) );
}

Vec3 Camera::GetAxisY() const
{
	return Normalize( Cross( Cross( m_vCenter - m_vEye, m_vUp), m_vCenter - m_vEye ) );
}

Vec3 Camera::GetAxisZ() const
{
	return Normalize(m_vCenter - m_vEye);
}

void Camera::BuildViewMatrix(Mat4 *m) const
{
	m->BuildViewMatrix(m_vEye, m_vCenter, m_vUp);
}

void Camera::MoveForward(float units)
{
	Vec3 vVector = m_vCenter - m_vEye;
	vVector = Normalize(vVector);

	m_vEye.x	+= vVector.x * units;
	m_vEye.z	+= vVector.z * units;

	m_vCenter.x	+= vVector.x * units;			
	m_vCenter.z	+= vVector.z * units;

	m_vEye.y	+= vVector.y * units;	
	m_vCenter.y	+= vVector.y * units;
}

void Camera::Strafe(float units)
{
	Vec3 x_axis = GetAxisX();

	m_vEye		+= x_axis * units;
	m_vCenter	+= x_axis * units;
}

void Camera::RotateView(float angle, float x, float y, float z)
{
	Mat4 m;

	m.BuildRotate( Vec3(x, y, z), angle );

	m_vCenter = m_vEye + m * (m_vCenter - m_vEye);
}

void Camera::SetViewByMouse(float angleY, float angleZ)
{
	static float currentRotX = 0.0f;
	static float lastRotX = 0.0f; 

 	lastRotX = currentRotX;
	
	currentRotX += angleZ;

	if(currentRotX > 89.0f)			// verifica se o ângulo não é maior que o ângulo max
	{
		currentRotX = 89.0f;
		
		if(lastRotX != 89.0f)		// se for verifica se há diferença entre a última rotação
		{
			Vec3 vAxis = Cross(m_vCenter - m_vEye, m_vUp);	// calcula o eixo de rotação
			vAxis = Normalize(vAxis);						// e o seu versor

			RotateView(89.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	else if(currentRotX < -89.0f)	// verifica se o ângulo não é maior que o ângulo max
	{

		currentRotX = -89.0f;
		
		if(lastRotX != -89.0f)		// se for verifica se há diferença entre a última rotação
		{
			Vec3 vAxis = Cross(m_vCenter - m_vEye, m_vUp);	// calcula o eixo de rotação
			vAxis = Normalize(vAxis);					// e o seu versor
			
			RotateView(-89.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	else 
	{	// executa a rotação normalmente
		Vec3 vAxis = Cross(m_vCenter - m_vEye, m_vUp);	// calcula o eixo de rotação
		vAxis = Normalize(vAxis);					// e o seu versor

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// executa sempre a rotação em torno do eixo dos yy
	RotateView(angleY, this->m_vUp.x, this->m_vUp.y, this->m_vUp.z);
}

/*
void Camera::SetViewByMouse()
{
	POINT mousePos;
	float angleY = 0.0f;
	float angleZ = 0.0f;
	static float currentRotX = 0.0f;
	static float lastRotX = 0.0f; 

	GetCursorPos(&mousePos);						
	
	if( (mousePos.x == g_OpenGLApp->m_winCenterX) && (mousePos.y == g_OpenGLApp->m_winCenterY) )
		return;

	g_OpenGLApp->CenterCursor();

	angleY = (float)( (g_OpenGLApp->m_winCenterX - mousePos.x) ) * 0.2f;
	angleZ = (float)( (g_OpenGLApp->m_winCenterY - mousePos.y) ) * 0.2f;
		
 	lastRotX = currentRotX;
	
	currentRotX += angleZ;

	if(currentRotX > 89.0f)			// verifica se o ângulo não é maior que o ângulo max
	{
		currentRotX = 89.0f;
		
		if(lastRotX != 89.0f)		// se for verifica se há diferença entre a última rotação
		{
			Vec3 vAxis = Cross(m_vCenter - m_vEye, m_vUp);	// calcula o eixo de rotação
			vAxis = Normalize(vAxis);					// e o seu versor

			RotateView(89.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	else if(currentRotX < -89.0f)	// verifica se o ângulo não é maior que o ângulo max
	{

		currentRotX = -89.0f;
		
		if(lastRotX != -89.0f)		// se for verifica se há diferença entre a última rotação
		{
			Vec3 vAxis = Cross(m_vCenter - m_vEye, m_vUp);	// calcula o eixo de rotação
			vAxis = Normalize(vAxis);					// e o seu versor
			
			RotateView(-89.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	else 
	{	// executa a rotação normalmente
		Vec3 vAxis = Cross(m_vCenter - m_vEye, m_vUp);	// calcula o eixo de rotação
		vAxis = Normalize(vAxis);					// e o seu versor

		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// executa sempre a rotação em torno do eixo dos yy
	RotateView(angleY, this->m_vUp.x, this->m_vUp.y, this->m_vUp.z);
}
*/