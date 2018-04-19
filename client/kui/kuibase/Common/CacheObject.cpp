#include "OpenHMI.h"

namespace OpenHMI
{
	static UINT g_uiTime = 0;

	CacheObject::CacheObject()
		: m_uiFrequency(0)
		, m_iID(-1)
	{
		g_uiTime++;

		m_uiTime = g_uiTime;
	}

	CacheObject::~CacheObject()
	{
		;
	}
}