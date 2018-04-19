/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../BaseProject/Framework/CBaseMessage.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgHardKey : public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CMsgHardKey(UINT uiKeyCode, INT iKeyStatus);
		virtual ~CMsgHardKey();

		virtual void	handleMessage();

	protected:// method
		CMsgHardKey(const CMsgHardKey&){}
		CMsgHardKey& operator =(const CMsgHardKey&){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT	m_uiKeyCode;
		INT		m_iKeyStatus;
	};
}