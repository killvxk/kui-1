#include "OpenHMI.h"

namespace OpenHMI
{
	CKeyTable::CKeyTable()
		: m_uiKeyType(-1)
		, m_uiKeyCode(-1)
	{
		;
	}

	CKeyTable::~CKeyTable()
	{
		;
	}

	void CKeyTable::setKeyType(UINT uiType)
	{
		m_uiKeyType = uiType;
	}

	void CKeyTable::setKeyCode(UINT uiCode)
	{
		m_uiKeyCode = uiCode;
	}
}