#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseHitArea::CBaseHitArea() : m_eHTAType(HTA_RECTANGLE)
	{
		;
	}

	CBaseHitArea::~CBaseHitArea()
	{
		;
	}

	BOOLEAN CBaseHitArea::hitTest(INT iX, INT iY)
	{
		return FALSE;
	}
}