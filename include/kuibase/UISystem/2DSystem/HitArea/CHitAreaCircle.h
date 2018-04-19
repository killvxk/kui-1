/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CHitAreaCircle : public CBaseHitArea
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CHitAreaCircle();
		virtual ~CHitAreaCircle();

		void		setCenterPoint(INT iX, INT iY);
		void		setRadius(UINT uiValue)					{ m_uiRadius = uiValue; }

		virtual BOOLEAN	hitTest(INT iX, INT iY);

	protected:// method
		
	private:// method

	protected:// property

	private:// property
		INT			m_iX;
		INT			m_iY;
		UINT		m_uiRadius;
	};
}