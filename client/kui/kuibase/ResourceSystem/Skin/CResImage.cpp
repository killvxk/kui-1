#include "OpenHMI.h"

namespace OpenHMI
{
	CResImage::CResImage()
		: m_bIsCommonImage(FALSE)
	{
		m_propertySet.setProperty(L"src", STRING_NULL);
		m_propertySet.setProperty(L"iscmn", FALSE);
	}

	CResImage:: ~CResImage()
	{		
		;
	}

	UINT CResImage::getImageSize()
	{
		return m_aryImage.size();
	}

	String CResImage::getImage(UINT uiIndex) const
	{
		String *pStr =  m_aryImage[uiIndex];
		if (pStr != NULL)
		{
			return (getName() + L"/" + *pStr);
		}
		else
		{
			return STRING_NULL;
		}
	}

	void CResImage::addImage(const String &strPath)
	{
		String *pstrPath = new String(strPath);
		m_aryImage.addItem(pstrPath);

		updateImageProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResImage::deleteImage(UINT uiIndex)
	{
		m_aryImage.deleteItemByIndex(uiIndex);

		updateImageProperty();

		CSkinParser::getInstance()->setDirty();
	}
	
	void CResImage::replaceImage(UINT uiIndex, const String &strPath)
	{
		m_aryImage.deleteItemByIndex(uiIndex);

		String *pstrPath = new String(strPath);
		m_aryImage.insertItem(uiIndex, pstrPath);

		updateImageProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResImage::setAsCommonImage()
	{
		m_bIsCommonImage = TRUE;

		m_propertySet.setProperty(L"iscmn", TRUE);

		CSkinParser::getInstance()->setDirty();
	}

	void CResImage::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);

		getImageList();

		m_bIsCommonImage = m_propertySet.getPropertyBool(L"iscmn");
	}

	String CResImage::saveProperty()
	{
		return CResNode::saveProperty();
	}

	void CResImage::notifyPropertyChanged()
	{
		CResNode::notifyPropertyChanged();

		getImageList();

		CSkinParser::getInstance()->setDirty();
	}

	void CResImage::getImageList()
	{
		String strSrc = CResNode::getPropertyString(L"src");
		CNodeParser::parseStrValue(strSrc, m_aryImage);
	}

	void CResImage::updateImageProperty()
	{
		String strContent = CNodeParser::saveStrValue(m_aryImage);
		m_propertySet.setProperty(L"src", strContent);
	}
}