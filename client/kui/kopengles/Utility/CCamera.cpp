#include "RenderEngine.h"

namespace OpenHMI
{

	const FLOAT CCamera::DEFAULT_FOVX = 90.0f;
	const FLOAT CCamera::DEFAULT_ZFAR = 1000.0f;
	const FLOAT CCamera::DEFAULT_ZNEAR = 0.1f;


	const C3DVector CCamera::WORLD_XAXIS(1.0f, 0.0f, 0.0f);
	const C3DVector CCamera::WORLD_YAXIS(0.0f, 1.0f, 0.0f);
	const C3DVector CCamera::WORLD_ZAXIS(0.0f, 0.0f, 1.0f);

	CCamera::CCamera()
	{
		m_fovx = DEFAULT_FOVX;
		m_znear = DEFAULT_ZNEAR;
		m_zfar = DEFAULT_ZFAR;
		m_aspectRatio = 0.0f;
	    
		m_posAt.set(0.0f, 0.0f, 0.0f);
		m_lookAt.set(0.0f, 0.0f, 0.0f);
		m_xAxis.set(1.0f, 0.0f, 0.0f);
		m_yAxis.set(0.0f, 1.0f, 0.0f);
		m_zAxis.set(0.0f, 0.0f, 1.0f);
		m_viewDir.set(0.0f, 0.0f, -1.0f);

		m_viewMatrix.identity();
		m_projectMatrix.identity();
	}

	CCamera::~CCamera()
	{
		;
	}

	void CCamera::lookAt(const C3DVector &target)
	{
		 lookAt(m_posAt, target, m_yAxis);
	}

	void CCamera::lookAt(const C3DVector &eye, const C3DVector &target, const C3DVector &up)
	{
		m_posAt = eye;
		m_lookAt = target;
		m_zAxis = eye - target;
		m_zAxis.normalise();

		m_viewDir = -m_zAxis;

		m_xAxis = up.crossProduct(m_zAxis);//C3DVector3::cross(up, m_zAxis);
		m_xAxis.normalise();

		m_yAxis = m_zAxis.crossProduct(m_xAxis);//C3DVector3::cross(m_zAxis, m_xAxis);
		m_yAxis.normalise();

		m_viewMatrix[0][0] = m_xAxis.getX();
		m_viewMatrix[1][0] = m_xAxis.getY();
		m_viewMatrix[2][0] = m_xAxis.getZ();
		m_viewMatrix[3][0] = -m_xAxis.dotProduct(eye);//-C3DVector3::dot(m_xAxis, eye);

		m_viewMatrix[0][1] = m_yAxis.getX();
		m_viewMatrix[1][1] = m_yAxis.getY();
		m_viewMatrix[2][1] = m_yAxis.getZ();
		m_viewMatrix[3][1] = -m_yAxis.dotProduct(eye);//-C3DVector3::dot(m_yAxis, eye);

		m_viewMatrix[0][2] = m_zAxis.getX();
		m_viewMatrix[1][2] = m_zAxis.getY();
		m_viewMatrix[2][2] = m_zAxis.getZ();    
		m_viewMatrix[3][2] = -m_zAxis.dotProduct(eye);//-C3DVector3::dot(m_zAxis, eye);
	}

	void CCamera::lookAt(FLOAT ex, FLOAT ey, FLOAT ez, FLOAT tx, FLOAT ty, FLOAT tz)
	{
		C3DVector eye(ex, ey, ez);
		C3DVector target(tx, ty, tz);
		C3DVector y(0.0f, 1.0f, 0.0f);
		lookAt(eye, target, y);
	}

	void CCamera::lookAt(FLOAT ex, FLOAT ey, FLOAT ez, FLOAT tx, FLOAT ty, FLOAT tz, FLOAT rx, FLOAT ry, FLOAT rz)
	{
	}

	void CCamera::setPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		m_posAt.set(x,y,z);
	}

	void CCamera::perspective(FLOAT fovx, FLOAT aspect, FLOAT znear, FLOAT zfar)
	{
		m_fovx = fovx;
		m_aspectRatio = aspect;
		m_znear = znear;
		m_zfar = zfar;

		updateProjectMatrix();
	}

	void CCamera::orthognal(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f)
	{
		m_projectMatrix.makeOrthognal(l,r,t,b,n,f);
	}

	void CCamera::rotate(FLOAT headingDegrees, FLOAT pitchDegrees, FLOAT rollDegrees)
	{
		;
	}

	void CCamera::updateProjectMatrix()
	{
		m_projectMatrix.makePerspective(m_fovx,m_aspectRatio,m_znear,m_zfar);
	}
}