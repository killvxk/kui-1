#include "RenderEngine.h"

namespace OpenHMI
{

	CFltReflection::CFltReflection()
	{
		;
	}

	CFltReflection::~CFltReflection()
	{
		;
	}

	void CFltReflection::doFilter(CDIB &srcDib, CDIB &bgDib, INT &iBgX, INT &iBgY, const CRenderFltReflection *pRndFilter)
	{
		if (pRndFilter->getHeight() == 0)
		{
			return;
		}	

		UINT uiHeight = pRndFilter->getHeight() <= srcDib.getHeight() ? pRndFilter->getHeight() : srcDib.getHeight();

		CDIB reflectionDib;
		reflectionDib.copyDIBFrom(&srcDib, 0, srcDib.getHeight() - uiHeight, srcDib.getWidth(), uiHeight);
		reflectionDib.flipVertically();

		CCacheImagePtr ptrImage(CImagePool::getInstance()->getImage(pRndFilter->getMaskImage()));
		if (ptrImage.isNotNull() )
		{
			reflectionDib.merge32DIB(0, 0, reflectionDib.getWidth(), reflectionDib.getHeight(),
									 ptrImage->getPixelData(), ptrImage->getWidth(), ptrImage->getHeight(), 255,
									 CDIB::HL_LEFT, CDIB::VL_TOP);
		}

		CDIB destDib;
		destDib.createDIB(srcDib.getWidth(), srcDib.getHeight() + uiHeight, srcDib.getPixelData(), srcDib.getStride() * srcDib.getHeight());
		destDib.replace(0, srcDib.getHeight(), reflectionDib.getWidth(), reflectionDib.getHeight(), reflectionDib.getPixelData());

		srcDib.moveDIBFrom(&destDib);
	}
}