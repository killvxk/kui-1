#include "OpenHMI.h"

namespace OpenHMI
{
	const String&	CLanguage::getString(UINT uiID)
	{
		return CLanguageParser::getInstance()->getString(uiID);
	}
}