#include "OpenHMI.h"

namespace OpenHMI
{
	CResHitArea::CResHitArea()
	{
		;
	}

	CResHitArea:: ~CResHitArea()
	{		
		;
	}

	CResHitArea* CResHitArea::clone()
	{
		CResHitArea *pNew = new CResHitArea();

		cloneTo(pNew);
		return pNew;	
	}

}