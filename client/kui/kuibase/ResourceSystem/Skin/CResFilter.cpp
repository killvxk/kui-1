#include "OpenHMI.h"

namespace OpenHMI
{
	CResFilter::CResFilter()
	{
		;
	}

	CResFilter:: ~CResFilter()
	{		
		;
	}

	CResFilter* CResFilter::clone()
	{
		CResFilter *pNew = new CResFilter();

		cloneTo(pNew);
		return pNew;	
	}

}