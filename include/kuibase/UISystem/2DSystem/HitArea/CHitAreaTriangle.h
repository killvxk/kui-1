/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CHitAreaTriangle : public CBaseHitArea
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CHitAreaTriangle();
		virtual ~CHitAreaTriangle();

		void setVertex1(INT iX, INT iY);
		void setVertex2(INT iX, INT iY);
		void setVertex3(INT iX, INT iY);

		virtual BOOLEAN	hitTest(INT iX, INT iY);

	protected:// method
		
	private:// method
		BOOLEAN changeVertexOrder();
		BOOLEAN	isSameSide(C3DVector vA, C3DVector vB, C3DVector vC, C3DVector vP);

	protected:// property

	private:// property
		INT			m_iX1;
		INT			m_iY1;
		INT			m_iX2;
		INT			m_iY2;
		INT			m_iX3;
		INT			m_iY3;
	};
}