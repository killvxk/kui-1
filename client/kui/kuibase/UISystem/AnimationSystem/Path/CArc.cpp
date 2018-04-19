#include "OpenHMI.h"

namespace OpenHMI
{
	CArc::CArc()
	{
		;
	}

	CArc::~CArc()
	{
		
	}

	void CArc::smashToSegments(INT iSegmentCount)
	{
		// smash to point array
		
		calculateEdgeLength();
	}
}