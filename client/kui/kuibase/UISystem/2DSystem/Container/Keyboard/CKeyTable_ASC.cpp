#include "OpenHMI.h"

namespace OpenHMI
{
	CKeyTable_ASC::CKeyTable_ASC()
	{
		;
	}

	CKeyTable_ASC::~CKeyTable_ASC()
	{
		;
	}

	String CKeyTable_ASC::getKeyCodeCharacter(BOOLEAN bUseLowercase)
	{
		if (isLetterKeyCode() && bUseLowercase)
		{
			return String::format(L"%c", m_uiKeyCode + 32);
		}

		if (isFuncKeyCode())
		{
			return L"";
		}

		return String::format(L"%c", m_uiKeyCode);
	}
}