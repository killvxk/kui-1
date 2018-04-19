#include "OpenHMI.h"

namespace OpenHMI
{
	CCircle::CCircle()
	{
		;
	}

	CCircle::~CCircle()
	{
		
	}

	void CCircle::smashToSegments(INT iSegmentCount)
	{
		// smash to point array
		
		calculateEdgeLength();
	}
}