#include "OpenHMI.h"

namespace OpenHMI
{
	CResImageField::CResImageField()
	{
		;
	}

	CResImageField:: ~CResImageField()
	{		
		;
	}

	CResImageField* CResImageField::clone()
	{
		CResImageField *pNew = CSkinParser::getInstance()->createImageField();

		UINT uiID = pNew->getID();

		cloneTo(pNew);

		pNew->setID(uiID);

		return pNew;
	}

	void CResImageField::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);
	}

	String CResImageField::saveProperty()
	{
		return CResNode::saveProperty();
	}
}