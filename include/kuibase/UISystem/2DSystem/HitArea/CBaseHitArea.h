/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseHitArea : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EHTAType
		{
			HTA_RECTANGLE = 0,
			HTA_TRIANGLE,
			HTA_CIRCLE,
		};

	private:// embed class or struct or enum

	public:// method
		virtual ~CBaseHitArea();

		virtual BOOLEAN	hitTest(INT iX, INT iY);

	protected:// method
		CBaseHitArea();
		
	private:// method

	protected:// property
		EHTAType		m_eHTAType;

	private:// property
		
	};
}