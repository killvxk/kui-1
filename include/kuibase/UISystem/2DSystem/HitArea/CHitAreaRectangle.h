/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CHitAreaRectangle : public CBaseHitArea
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CHitAreaRectangle();
		virtual ~CHitAreaRectangle();

		void setX(INT iValue)						{ m_iX = iValue; }
		void setY(INT iValue)						{ m_iY = iValue; }
		void setWidth(UINT uiValue)					{ m_uiWidth = uiValue; }
		void setHeight(UINT	uiValue)				{ m_uiHeight = uiValue; }

		virtual BOOLEAN	hitTest(INT iX, INT iY);

	protected:// method
		
	private:// method

	protected:// property

	private:// property
		INT			m_iX;
		INT			m_iY;
		UINT		m_uiWidth;
		UINT		m_uiHeight;
	};
}