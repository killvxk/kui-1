/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"
#include "GdiPlus.h"

namespace OpenHMI
{
	class OPENHMI_API IRenderEngine
	{
	public:// method
		virtual					~IRenderEngine() {}

		virtual	BOOLEAN			initialize(const tagScreenContext *pScreenContext = NULL) = 0;
		virtual	void			unInitialize(const tagScreenContext *pScreenContext = NULL) = 0;
		virtual IRenderSession*	getRenderSession(UINT uiID) = 0;
		virtual void			destroyRenderSession(UINT uiID) = 0;
		
		virtual	void			releaseCacheData() = 0;
		virtual	void			getImageInfo(const String &strFilePath, UINT *puiWidth, UINT *puiHeight) = 0;
		virtual void			getImageInfo(const String &strID, Gdiplus::Bitmap* pBitmap, UINT *puiWidth, UINT *puiHeight) = 0;
		virtual	void			getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight) = 0;
	};
}