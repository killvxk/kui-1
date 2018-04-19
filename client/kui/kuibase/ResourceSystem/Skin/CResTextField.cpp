#include "OpenHMI.h"

namespace OpenHMI
{
	CResTextField::CResTextField()
	{
		;
	}

	CResTextField:: ~CResTextField()
	{		
		;
	}

	CResTextField* CResTextField::clone()
	{
		CResTextField *pNew = CSkinParser::getInstance()->createTextField();

		UINT uiID = pNew->getID();

		cloneTo(pNew);

		pNew->setID(uiID);

		return pNew;
	}

	void CResTextField::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);
	}

	String CResTextField::saveProperty()
	{
		return CResNode::saveProperty();
	}
}