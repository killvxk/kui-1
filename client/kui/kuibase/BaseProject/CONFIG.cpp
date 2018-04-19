#include "OpenHMI.h"

namespace OpenHMI
{
	BOOLEAN	CONFIG::s_bUseCCP = FALSE;
	BOOLEAN	CONFIG::s_bRunOnTarget = FALSE;

	void CONFIG::checkRunOnTarget()
	{
#ifdef OS_WIN
		s_bRunOnTarget = FALSE;
#endif

#ifdef OS_QNX
		s_bRunOnTarget = TRUE;
#endif

#ifdef OS_LINUX
		s_bRunOnTarget = TRUE;
#endif
	}
}