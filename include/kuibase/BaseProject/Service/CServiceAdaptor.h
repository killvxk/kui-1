/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CServiceAdaptor : public CThread
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CBaseMainFrame;

	public:// method	
		static	CServiceAdaptor*	getInstance();
		static	void				deleteInstance();

		/**
		*  Inner method.
		*/
		BOOLEAN		registerServiceAdaptor(const String &strFileName, BOOLEAN bInitialize = FALSE);

		/**
		*  Inner method.
		*/
		void		pushInvokeMessage(CBaseMessage *pMessage);
		
	protected:// method
		CServiceAdaptor();
		virtual ~CServiceAdaptor();

		CBaseMessage*	popMessage();
		void			clearMessageList();

		virtual	void	run();

	private:// method
		BOOLEAN		loadLibrary(const String &pstrFileName);
		void		freeLibrary();	
		BOOLEAN		initialize();
		void		unInitialize();

	protected:// property

	private:// property
		static	CServiceAdaptor	*s_pInstance;

		IServiceAdaptor			*m_pServiceAdaptor;

		List<CBaseMessage>		m_listMessage;

#ifdef OS_WIN
		HINSTANCE				m_hInst;
#endif

#ifdef OS_QNX
		void*					m_dllHandle;
#endif

#ifdef OS_LINUX
		void*					m_dllHandle;
#endif
	};
}