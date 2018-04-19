/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgFromServiceThread : public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum		

	public:// method	
		CMsgFromServiceThread(CBaseService *pService, UINT uiMethodSignalID, Object *pData, BOOLEAN bIsSignal);
		virtual ~CMsgFromServiceThread();

		virtual void	handleMessage();

	protected:// method
		CMsgFromServiceThread(const CMsgFromServiceThread&){}
		CMsgFromServiceThread& operator =(const CMsgFromServiceThread&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		CBaseService	*m_pService;
		UINT			m_uiMethodSignalID;
		Object			*m_pData;
		BOOLEAN			m_bIsSignal;
	};
}