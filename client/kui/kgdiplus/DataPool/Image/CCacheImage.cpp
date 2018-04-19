#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheImage::CCacheImage(UINT uiWidth, UINT uiHeihgt, BYTE *pbtPixelData)
		: m_uiWidth(uiWidth)
		, m_uiHeihgt(uiHeihgt)
		, m_pbtPixelData(pbtPixelData)
	{
		;
	}

	CCacheImage::~CCacheImage()
	{
		if (m_pbtPixelData != NULL) 
		{
			delete [] m_pbtPixelData;
			m_pbtPixelData = NULL;
		}
	}
}