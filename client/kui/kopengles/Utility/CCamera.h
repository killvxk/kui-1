#pragma once

namespace OpenHMI
{
	class CCamera
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCamera();
		virtual ~CCamera();

		void lookAt(const C3DVector &target);
		void lookAt(const C3DVector &eye, const C3DVector &target, const C3DVector &up);
		void lookAt(FLOAT ex, FLOAT ey, FLOAT ez, FLOAT tx, FLOAT ty, FLOAT tz);
		void lookAt(FLOAT ex, FLOAT ey, FLOAT ez, FLOAT tx, FLOAT ty, FLOAT tz, FLOAT rx, FLOAT ry, FLOAT rz);
		void setPosition(FLOAT x, FLOAT y, FLOAT z);
	
		void perspective(FLOAT fovx, FLOAT aspect, FLOAT znear, FLOAT zfar);
		void orthognal(FLOAT l, FLOAT r, FLOAT t, FLOAT b, FLOAT n, FLOAT f);
		void rotate(FLOAT headingDegrees, FLOAT pitchDegrees, FLOAT rollDegrees);
		
		// Getter methods.
		const C3DVector &getPosition() const			{ return m_posAt;			}
		const C3DMatrix &getProjectMatrix() const		{ return m_projectMatrix;	}
		const C3DMatrix &getViewMatrix() const			{ return m_viewMatrix;		}
		const C3DVector &getXAxis() const				{ return m_xAxis;			}
		const C3DVector &getYAxis() const				{ return m_yAxis;			}
		const C3DVector &getZAxis() const				{ return m_zAxis;			}

	protected:// method
		CCamera(const CCamera&)	{}
		CCamera &operator = (const CCamera &){return *this;}

	private:// method
		void updateViewMatrix();
		void updateProjectMatrix();

	protected:// property

	private:// property
		static const FLOAT		DEFAULT_FOVX;
		static const FLOAT		DEFAULT_ZFAR;
		static const FLOAT		DEFAULT_ZNEAR;
		static const C3DVector	WORLD_XAXIS;
		static const C3DVector	WORLD_YAXIS;
		static const C3DVector	WORLD_ZAXIS;

		FLOAT			m_fovx;
		FLOAT			m_znear;
		FLOAT			m_zfar;
		FLOAT			m_aspectRatio;
		FLOAT			m_accumPitchDegrees;

		C3DVector		m_posAt;
		C3DVector		m_lookAt;
		C3DVector		m_xAxis;
		C3DVector		m_yAxis;
		C3DVector		m_zAxis;
		C3DVector		m_viewDir;
		
		C3DMatrix		m_viewMatrix;
		C3DMatrix		m_projectMatrix;
	};
}