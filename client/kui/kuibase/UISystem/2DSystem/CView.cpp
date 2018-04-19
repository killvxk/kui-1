#include "OpenHMI.h"

namespace OpenHMI
{
	CView::CView()
		: m_uiImageID(0)
	{
		;
	}

	CView::~CView()
	{
		;
	}

	CMovieClip*	CView::getSkin()
	{
		return ENV::getMovieClip(m_uiImageID);
	}
}