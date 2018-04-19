/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CGestureManager
		: public Object
		, public CEventDispatcher
		, public IGestureRecognizer
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CBaseMainFrame;

	public:// method	
		static	CGestureManager*	getInstance();
		static	void				deleteInstance();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		BOOLEAN		registerGestureRecognizer(const String &strFileName);
		BOOLEAN		registerGestureRecognizer(IGestureRecognizer *pGestureRecognizer) { m_pGestureRecognizer = pGestureRecognizer ; return TRUE; }

		void	dispatchEvent(UINT uiEventID, Object *pData);

		void	addCursor(UINT uiGroupID, const CCursor &cursor);	
		void	updateCursor(UINT uiGroupID, const CCursor &cursor);				
		void	removeCursor(UINT uiGroupID, const CCursor &cursor);				
		
	protected:// method
		CGestureManager();
		virtual ~CGestureManager();

	private:// method
		BOOLEAN		loadLibrary(const String &pstrFileName);
		void		freeLibrary();	

	protected:// property

	private:// property
		static	CGestureManager	*s_pInstance;

		IGestureRecognizer		*m_pGestureRecognizer;

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