#include "RenderEngine.h"

namespace OpenHMI
{

	CFltMask::CFltMask()
	{
		;
	}

	CFltMask::~CFltMask()
	{
		;
	}

	void CFltMask::doFilter(CDIB &srcDib, CDIB &bgDib, INT &iBgX, INT &iBgY, const CRenderFltMask *pRndFilter)
	{
		CCacheImagePtr ptrImage(CImagePool::getInstance()->getImage(pRndFilter->getMaskImage()));
		if (ptrImage.isNotNull() )
		{
			srcDib.mergeMaskDIB(ptrImage->getPixelData(), ptrImage->getWidth(), ptrImage->getHeight());
		}
	}
}