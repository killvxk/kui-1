/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CMain 
		: public CThread
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum		

	private:// embed class or struct or enum

	public:// method
		static	CMain*	getInstance();

		void	startHMI(CBaseMainFrame	*pMainFrame);
		void	quitHMI();
		void	paintHMI();
		
		void	pushMessage(CBaseMessage *pMessage);
		void	deleteMessageByID(UINT uiID);

	protected:// method
		CMain();
		virtual ~CMain();

		void			pushRenderMessage(List<CBaseMessage> &listMessage, CBaseMessage *pMessage);
		void			deleteMessageByID(List<CBaseMessage> &listMessage, UINT uiID);
		CBaseMessage*	popMessage();
		void			clearMessageList();

		virtual	void	run();

	private:// method

	protected:// property	

	private:// property
		static	CMain	*s_pInstance;

		List<CBaseMessage>	m_listInputMessage;		// 1
		List<CBaseMessage>	m_listServiceMessage;	// 2
		List<CBaseMessage>	m_listTimerMessage;		// 3
		List<CBaseMessage>	m_listUnknownMessage;	// 4
		UINT				m_uiActiveMessageList;
	};
}