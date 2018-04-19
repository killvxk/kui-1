#include "OpenHMI.h"

namespace OpenHMI
{
	CResFont::CResFont() 
		: m_strFontName(STRING_NULL)
		, m_uiFontSize(24)
		, m_uiFontColor(0xFF000000)
		, m_strFontStyle(STRING_NULL)
	{
		m_propertySet.setProperty(L"fontname", m_strFontName);
		m_propertySet.setProperty(L"size", m_uiFontSize);		
		m_propertySet.setProperty(L"color", m_uiFontColor);
		m_propertySet.setProperty(L"style", m_uiFontColor);
	}

	CResFont:: ~CResFont()
	{		
		;
	}

	void CResFont::setFontName(const String &strName)
	{
		m_strFontName = strName;

		m_propertySet.setProperty(L"fontname", m_strFontName);

		CSkinParser::getInstance()->setDirty();
	}

	void CResFont::setFontSize(UINT uiSize)
	{
		m_uiFontSize = uiSize;

		m_propertySet.setProperty(L"size", m_uiFontSize);	

		CSkinParser::getInstance()->setDirty();
	}

	void CResFont::setFontColor(UINT uiColor)
	{
		m_uiFontColor = uiColor;

		m_propertySet.setProperty(L"color", m_uiFontColor);

		CSkinParser::getInstance()->setDirty();
	}

	void CResFont::setFontStyle(const String &strStyle)
	{
		m_strFontStyle = strStyle;

		m_propertySet.setProperty(L"style", m_strFontStyle);

		CSkinParser::getInstance()->setDirty();
	}

	void CResFont::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);
		
		updateFontInfo();
	}

	CResFont* CResFont::clone()
	{
		CResFont *pNew = CSkinParser::getInstance()->createFont();

		UINT uiID = pNew->getID();

		cloneTo(pNew);

		pNew->setID(uiID);

		return pNew;
	}

	String CResFont::saveProperty()
	{
		return CResNode::saveProperty();
	}

	void CResFont::notifyPropertyChanged()
	{
		CResNode::notifyPropertyChanged();

		updateFontInfo();
	}

	void CResFont::updateFontInfo()
	{
		m_strFontName = CResNode::getPropertyString(L"fontname");
		m_uiFontSize = CResNode::getPropertyUInt(L"size");
		m_uiFontColor = CResNode::getPropertyUInt(L"color");
		m_strFontStyle = CResNode::getPropertyString(L"style");
	}
}