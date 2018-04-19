#include "OpenHMI.h"

namespace OpenHMI
{
	CBezier::CBezier()
	{
		;
	}

	CBezier::~CBezier()
	{
		
	}

	void CBezier::smashToSegments(INT iSegmentCount)
	{
		// smash to point array
		
		calculateEdgeLength();
	}
}