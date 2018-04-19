#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseMessage::CBaseMessage()
		: m_eType(MT_UNKNOWN)
		, m_uiID(0)
	{
		;
	}

	CBaseMessage::~CBaseMessage()
	{
		;
	}
}