#include "OpenHMI.h"

namespace OpenHMI
{
	CResFontGroup::CResFontGroup() 
		: m_pCurFont(NULL)
	{
		;
	}

	CResFontGroup:: ~CResFontGroup()
	{		
		;
	}

	const String& CResFontGroup::getFontName() const
	{
		if (m_pCurFont != NULL)
		{
			return m_pCurFont->getFontName();
		}
		else
		{
			return STRING_NULL;
		}
	}

	UINT CResFontGroup::getFontSize() const
	{
		if (m_pCurFont != NULL)
		{
			return m_pCurFont->getFontSize();
		}
		else
		{
			return 24;
		}
	}

	UINT CResFontGroup::getFontColor() const
	{
		if (m_pCurFont != NULL)
		{
			return m_pCurFont->getFontColor();
		}
		else
		{
			return 0xFF000000;
		}
	}

	const String& CResFontGroup::getFontStyle() const
	{
		if (m_pCurFont != NULL)
		{
			return m_pCurFont->getFontStyle();
		}
		else
		{
			return STRING_NULL;
		}
	}

	UINT CResFontGroup::getFontCount()
	{
		return m_aryFont.size();
	}

	CResFont* CResFontGroup::getFontByIndex(UINT uiIndex)
	{
		return CSkinParser::getInstance()->getFontByID(m_aryFont[uiIndex]);
	}

	CResFont* CResFontGroup::createFont()
	{
		CResFont *pFont = CSkinParser::getInstance()->createFont();

		m_aryFont.addItem(pFont->getID());

		updateFontProperty();
		updateFontInfo();

		CSkinParser::getInstance()->setDirty();

		return pFont;
	}

	void CResFontGroup::deleteFont(CResFont *pFont)
	{
		if (pFont == NULL)
		{
			return;
		}

		m_aryFont.deleteItem(pFont->getID());

		CSkinParser::getInstance()->deleteFont(pFont);

		updateFontProperty();
		updateFontInfo();

		CSkinParser::getInstance()->setDirty();
	}

	void CResFontGroup::removeFont(CResFont *pFont)
	{
		if (pFont == NULL)
		{
			return;
		}

		m_aryFont.deleteItem(pFont->getID());

		updateFontProperty();
		updateFontInfo();

		CSkinParser::getInstance()->setDirty();
	}

	void CResFontGroup::addFont(CResFont *pFont)
	{
		if (pFont == NULL)
		{
			return;
		}

		m_aryFont.addItem(pFont->getID());

		updateFontProperty();
		updateFontInfo();

		CSkinParser::getInstance()->setDirty();
	}

	void CResFontGroup::deleteAllFont()
	{
		m_aryFont.resetIterator();
		while (m_aryFont.hasMore())
		{
			CSkinParser::getInstance()->deleteFont(CSkinParser::getInstance()->getFontByID(m_aryFont.next()));
		}
		m_aryFont.clear();

		updateFontProperty();
		updateFontInfo();

		CSkinParser::getInstance()->setDirty();
	}

	void CResFontGroup::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);
		
		getFontList();
		updateFontInfo();
	}

	String CResFontGroup::saveProperty()
	{
		return CResNode::saveProperty();
	}

	void CResFontGroup::notifyPropertyChanged()
	{
		CResNode::notifyPropertyChanged();

		getFontList();
		updateFontInfo();

		CSkinParser::getInstance()->setDirty();
	}

	void CResFontGroup::getFontList()
	{
		String strFont = CResNode::getPropertyString(L"fonts");
		CNodeParser::parseUIntValue(strFont, m_aryFont);
	}

	void CResFontGroup::updateFontProperty()
	{
		String strContent = CNodeParser::saveUIntValue(m_aryFont);
		m_propertySet.setProperty(L"fonts", strContent);
	}

	void CResFontGroup::updateFontInfo()
	{
		if (m_aryFont.size() > 0)
		{
			m_pCurFont = CSkinParser::getInstance()->getFontByID(m_aryFont[0]);
		}
		else
		{
			m_pCurFont = NULL;
		}
	}
}