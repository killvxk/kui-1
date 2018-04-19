/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderEngine
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CMain;
		friend class CBaseMainFrame;

	public:// method		
		static	CRenderEngine*	getInstance();
		static	void			deleteInstance();

		BOOLEAN			registernRenderEngine(const String &strFileName, BOOLEAN bInitialize = FALSE);			
		
		IRenderSession*	getRenderSession(UINT uiID);
		void			destroyRenderSession(UINT uiID);

		void			releaseCacheData();
		void			getImageInfo(const String &strFilePath, UINT *puiWidth, UINT *puiHeight);
		void			getImageInfo(const String &strFilePath, Gdiplus::Bitmap* pBitmap, UINT *puiWidth, UINT *puiHeight);
		void			getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight);

	protected:// method
		CRenderEngine();
		virtual ~CRenderEngine();

	private:// method
		BOOLEAN		loadLibrary(const String &pstrFileName);
		void		freeLibrary();	
		BOOLEAN		initialize(const tagScreenContext *pScreenContext = NULL);
		void		unInitialize(const tagScreenContext *pScreenContext = NULL);

	protected:// property

	private:// property
		static	CRenderEngine	*s_pInstance;

		IRenderEngine			*m_pEngine;

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