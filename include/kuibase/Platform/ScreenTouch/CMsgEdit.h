/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../BaseProject/Framework/CBaseMessage.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgEdit 
		: public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CMsgEdit(UINT uiStatus);
		virtual ~CMsgEdit();

		virtual void	handleMessage();

	protected:// method
		CMsgEdit(const CMsgEdit&){}
		CMsgEdit& operator =(const CMsgEdit&){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT	m_uiStatus;
	};
}