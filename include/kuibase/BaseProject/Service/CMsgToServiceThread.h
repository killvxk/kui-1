/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgToServiceThread : public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum		

	public:// method
		CMsgToServiceThread(CBaseService *pService, UINT uiMethodID, Object *pData);
		virtual ~CMsgToServiceThread();

		virtual void	handleMessage();

	protected:// method
		CMsgToServiceThread(const CMsgToServiceThread&){}
		CMsgToServiceThread& operator =(const CMsgToServiceThread&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		CBaseService	*m_pService;
		UINT			m_uiMethodID;
		Object			*m_pData;
	};
}