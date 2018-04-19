/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgGesture : public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum		

	public:// method	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CMsgGesture(UINT uiGestureID, Object *pData);
		virtual ~CMsgGesture();

		virtual void	handleMessage();

	protected:// method
		CMsgGesture(const CMsgGesture&){}
		CMsgGesture& operator =(const CMsgGesture&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		UINT			m_uiGestureID;
		Object			*m_pData;
	};
}