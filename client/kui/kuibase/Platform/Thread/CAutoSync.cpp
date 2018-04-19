#include "OpenHMI.h"

namespace OpenHMI
{
	CAutoSync::CAutoSync(CSyncObject &syncObj) 
		: m_syncObj(syncObj)
	{
		m_syncObj.lock();
	}

	CAutoSync::~CAutoSync()
	{
		m_syncObj.unlock();
	}
}