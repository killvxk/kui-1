/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../BaseProject/Framework/CBaseMessage.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgScreen
		: public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CMsgScreen(UINT uiStatus);
		virtual ~CMsgScreen();

		virtual void	handleMessage();

	protected:// method
		CMsgScreen(const CMsgScreen&){}
		CMsgScreen& operator =(const CMsgScreen&){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT	m_uiStatus;
	};
}