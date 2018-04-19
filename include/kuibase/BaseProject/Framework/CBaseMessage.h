/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseMessage : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EMessageType
		{
			MT_UNKNOWN			= 0,
			MT_RENDER			= 1,
			MT_SCREEN_TOUCH		= 2,
			MT_HARD_KEY			= 3,			
			MT_FROM_SERVICE		= 4,
			MT_TO_SERVICE		= 5,
			MT_TIMER			= 6,
			MT_GESTURE			= 7,
			MT_SCREEN			= 8,
		};

	private:// embed class or struct or enum
		
	public:// method		
		virtual ~CBaseMessage();

		EMessageType	getType()	{ return m_eType; }

		UINT			getID()		{ return m_uiID; }

		virtual void	handleMessage() = 0;

	protected:// method
		CBaseMessage();

	private:// method

	protected:// property
		EMessageType	m_eType;
		UINT			m_uiID;

	private:// property

	};
}