#include "OpenHMI.h"

namespace OpenHMI
{
#define DEFAULT_SCREEN_WIDTH	800
#define	DEFAULT_SCREEN_HEIGHT	480
#define DEFAULT_SKIN_NAME		L"LanguageName_ColorName"

	CSkinParser	*CSkinParser::s_pInstance	= NULL;

	CSkinParser* CSkinParser::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CSkinParser();
		}

		return s_pInstance;
	}

	void CSkinParser::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CSkinParser::CSkinParser()
		: m_bDirty(FALSE)
		, m_pMergeSrc(NULL)
	{
		;
	}

	CSkinParser::~CSkinParser()
	{
		;
	}

	BOOLEAN CSkinParser::createSkin(const String &strPath)
	{
		m_strSkinPath = strPath;

		CHAR acFolder[1024] = {0};
		CPath::getFileFolder(m_strSkinPath, acFolder);
		m_strSkinFolder = acFolder;

		m_strPrivateImgFolder = m_strSkinFolder;

		m_strCommonImgFolder = CPath::getFileFolder(m_strSkinFolder.subString(0, m_strSkinFolder.length() - 1));
		
		initDefaultValue();

		return TRUE;
	}

	void CSkinParser::closeSkin()
	{
		m_strSkinPath = STRING_NULL;
		m_strSkinFolder = STRING_NULL;
		m_strPrivateImgFolder = STRING_NULL;
		m_strCommonImgFolder = STRING_NULL;

		m_strSkinName = STRING_NULL;

		m_mapScreen.clear();
		m_mapPopup.clear();
		m_mapFuncWidget.clear();
		m_mapCtrlWidget.clear();
		m_mapImage.clear();
		m_mapFontGroup.clear();
		m_mapFont.clear();
		m_mapTextField.clear();
		m_mapImageField.clear();
	}
	
	BOOLEAN CSkinParser::hasLoadSkin()
	{
		return m_strSkinPath.length() == 0 ? FALSE : TRUE;
	}	

	BOOLEAN CSkinParser::parse(const String &strPath)
	{
		CNodeParser parser;

		if (parser.parse(strPath) == FALSE)
		{
			return FALSE;
		}

		m_strSkinPath = strPath;

		CHAR acFolder[1024] = {0};
		CPath::getFileFolder(m_strSkinPath, acFolder);
		m_strSkinFolder = acFolder;

		m_strPrivateImgFolder = m_strSkinFolder;

		m_strCommonImgFolder = CPath::getFileFolder(m_strSkinFolder.subString(0, m_strSkinFolder.length() - 1));

		buildGlobalData(parser);
		buildImageData(parser);
		buildFontData(parser);
		buildFontGroupData(parser);
		buildTextFieldData(parser);
		buildImageFieldData(parser);		
		buildFuncWidgetData(parser);
		buildCtrlWidgetData(parser);
		buildScreenData(parser);
		buildPopupData(parser);

		clearDirty();

		return TRUE;
	}

	BOOLEAN CSkinParser::save(const String &strPath)
	{
		CNodeParser parser;
		BOOLEAN bSaved;

		saveScreen(parser);
		savePopup(parser);
		saveFuncWidget(parser);
		saveCtrlWidget(parser);
		saveImage(parser);
		saveFontGroup(parser);
		saveFont(parser);
		saveTextField(parser);
		saveImageField(parser);
		saveGlobal(parser);

		if (strPath.length() > 0)
		{
			bSaved = parser.save(strPath);
		}
		else
		{
			bSaved = parser.save(m_strSkinPath);
		}
		
		if (m_bDirty && bSaved)
		{
			clearDirty();
		}

		return bSaved;
	}

	BOOLEAN CSkinParser::merge(const String &strSkinPath)
	{
		if (m_strSkinPath == strSkinPath)
		{
			return FALSE;
		}

		m_pMergeSrc = new CSkinParser();
		if (!m_pMergeSrc->parse(strSkinPath))
		{
			delete m_pMergeSrc;
			m_pMergeSrc = NULL;
			return FALSE;
		}

		mergeScreen();
		mergePopup();
		mergeFuncWidget();
		mergeCtrlWidget();
		mergeTextField();
		mergeImageField();

		m_uiScreenWidth = m_pMergeSrc->m_uiScreenWidth;
		m_uiScreenHeight = m_pMergeSrc->m_uiScreenHeight;

		m_uiMaxScreenID = m_pMergeSrc->m_uiMaxScreenID;
		m_uiMaxPopupID = m_pMergeSrc->m_uiMaxPopupID;
		m_uiMaxFuncWidgetID = m_pMergeSrc->m_uiMaxFuncWidgetID;
		m_uiMaxCtrlWidgetID = m_pMergeSrc->m_uiMaxCtrlWidgetID;
		m_uiMaxTextFieldID = m_pMergeSrc->m_uiMaxTextFieldID;
		m_uiMaxImageFieldID = m_pMergeSrc->m_uiMaxImageFieldID;

		setDirty();

		return TRUE;
	}	

	const String& CSkinParser::getSkinPath() const
	{
		return m_strSkinPath;
	}

	const String& CSkinParser::getSkinFolder() const
	{
		return m_strSkinFolder;
	}

	const String& CSkinParser::getPrivateImageFolder() const
	{
		return m_strPrivateImgFolder;
	}

	const String& CSkinParser::getCommonImageFolder()
	{
		return m_strCommonImgFolder;
	}

	const String& CSkinParser::getSkinName() const
	{
		return m_strSkinName;
	}

	void CSkinParser::setSkinName(const String strValue)
	{
		m_strSkinName = strValue;
	}

	UINT CSkinParser::getScreenWidth() const
	{
		return m_uiScreenWidth;
	}

	void CSkinParser::setScreenWidth(UINT uiWidth)
	{
		m_uiScreenWidth = uiWidth;
	}

	UINT CSkinParser::getScreenHeight() const
	{
		return m_uiScreenHeight;
	}

	void CSkinParser::setScreenHeight(UINT uiHeight)
	{
		m_uiScreenHeight = uiHeight;
	}

	UINT CSkinParser::getScreenSize()
	{
		return m_mapScreen.size();
	}

	CResScreen* CSkinParser::getScreenByIndex(UINT uiIndex)
	{
		return m_mapScreen.getValueByIndex(uiIndex);
	}

	CResScreen* CSkinParser::getScreenByID(UINT uiID)
	{
		return m_mapScreen.getValueByKey(uiID);
	}

	CResScreen* CSkinParser::getScreenByName(const String &strName)
	{
		for (UINT uiIndex = 0; uiIndex < m_mapScreen.size(); ++uiIndex)
		{
			CResScreen* pResScreen = m_mapScreen.getValueByIndex(uiIndex);
			if (strName == pResScreen->getName())
			{
				return pResScreen;
			}
		}

		return NULL;
	}

	CResScreen* CSkinParser::createScreen()
	{
		CResScreen *pScreen = new CResScreen;

		m_uiMaxScreenID++;
		pScreen->setID(m_uiMaxScreenID);
		pScreen->setType(L"SCREEN");

		m_mapScreen.addItem(pScreen->getID(), pScreen);

		setDirty();

		return pScreen;
	}

	void CSkinParser::deleteScreen(CResScreen *pScreen)
	{
		if (pScreen == NULL)
		{
			return;
		}

		pScreen->deleteAllTextField();
		pScreen->deleteAllImageField();
		m_mapScreen.deleteItem(pScreen->getID());

		setDirty();
	}

	UINT CSkinParser::getPopupSize()
	{
		return m_mapPopup.size();
	}

	CResPopup* CSkinParser::getPopupByIndex(UINT uiIndex)
	{
		return m_mapPopup.getValueByIndex(uiIndex);
	}

	CResPopup* CSkinParser::getPopupByID(UINT uiID)
	{
		return m_mapPopup.getValueByKey(uiID);
	}	

	CResPopup* CSkinParser::getPopupByName(const String &strName)
	{
		for (UINT uiIndex = 0; uiIndex < m_mapPopup.size(); ++uiIndex)
		{
			CResPopup* pResPopup = m_mapPopup.getValueByIndex(uiIndex);
			if (strName == pResPopup->getName())
			{
				return pResPopup;
			}
		}

		return NULL;
	}

	CResPopup* CSkinParser::createPopup()
	{
		CResPopup *pPopup = new CResPopup;

		m_uiMaxPopupID++;
		pPopup->setID(m_uiMaxPopupID);
		pPopup->setType(L"POPUP");

		m_mapPopup.addItem(pPopup->getID(), pPopup);

		setDirty();

		return pPopup;
	}

	void CSkinParser::deletePopup(CResPopup *pPopup)
	{
		if (pPopup == NULL)
		{
			return;
		}

		pPopup->deleteAllTextField();
		pPopup->deleteAllImageField();
		m_mapPopup.deleteItem(pPopup->getID());

		setDirty();
	}

	UINT CSkinParser::getFuncWidgetSize()
	{
		return m_mapFuncWidget.size();
	}

	CResFuncWidget* CSkinParser::getFuncWidgetByIndex(UINT uiIndex)
	{
		return m_mapFuncWidget.getValueByIndex(uiIndex);
	}

	CResFuncWidget* CSkinParser::getFuncWidgetByID(UINT uiID)
	{
		return m_mapFuncWidget.getValueByKey(uiID);
	}	

	CResFuncWidget* CSkinParser::getFuncWidgetByName(const String &strName)
	{
		for (UINT uiIndex = 0; uiIndex < m_mapFuncWidget.size(); ++uiIndex)
		{
			CResFuncWidget* pResFuncWidget = m_mapFuncWidget.getValueByIndex(uiIndex);
			if (strName == pResFuncWidget->getName())
			{
				return pResFuncWidget;
			}
		}

		return NULL;
	}

	CResFuncWidget* CSkinParser::createFuncWidget()
	{
		CResFuncWidget *pWidget = new CResFuncWidget;

		m_uiMaxFuncWidgetID++;
		pWidget->setID(m_uiMaxFuncWidgetID);
		pWidget->setType(L"FUNCWIDGET");

		m_mapFuncWidget.addItem(pWidget->getID(), pWidget);

		setDirty();

		return pWidget;
	}

	void CSkinParser::deleteFuncWidget(CResFuncWidget *pFuncWidget)
	{
		if (pFuncWidget == NULL)
		{
			return;
		}

		pFuncWidget->deleteAllTextField();
		pFuncWidget->deleteAllImageField();
		m_mapFuncWidget.deleteItem(pFuncWidget->getID());

		setDirty();
	}
	
	UINT CSkinParser::getCtrlWidgetSize()
	{
		return m_mapCtrlWidget.size();
	}

	CResCtrlWidget* CSkinParser::getCtrlWidgetByIndex(UINT uiIndex)
	{
		return m_mapCtrlWidget.getValueByIndex(uiIndex);
	}

	CResCtrlWidget*	CSkinParser::getCtrlWidgetByID(UINT uiID)
	{
		return m_mapCtrlWidget.getValueByKey(uiID);
	}

	CResCtrlWidget* CSkinParser::getCtrlWidgetByName(const String &strName)
	{
		for (UINT uiIndex = 0; uiIndex < m_mapCtrlWidget.size(); ++uiIndex)
		{
			CResCtrlWidget* pResCtrlWidget = m_mapCtrlWidget.getValueByIndex(uiIndex);
			if (strName == pResCtrlWidget->getName())
			{
				return pResCtrlWidget;
			}
		}

		return NULL;
	}

	CResCtrlWidget*	CSkinParser::createCtrlWidget()
	{
		CResCtrlWidget *pWidget = new CResCtrlWidget;

		m_uiMaxCtrlWidgetID++;
		pWidget->setID(m_uiMaxCtrlWidgetID);
		pWidget->setType(L"CTRLWIDGET");

		m_mapCtrlWidget.addItem(pWidget->getID(), pWidget);

		setDirty();

		return pWidget;
	}

	void CSkinParser::deleteCtrlWidget(CResCtrlWidget *pCtrlWidget)
	{
		if (pCtrlWidget == NULL)
		{
			return;
		}

		pCtrlWidget->deleteAllTextField();
		pCtrlWidget->deleteAllImageField();
		m_mapCtrlWidget.deleteItem(pCtrlWidget->getID());

		setDirty();
	}

	UINT CSkinParser::getImageSize()
	{
		return m_mapImage.size();
	}

	CResImage* CSkinParser::getImageByIndex(UINT uiIndex)
	{
		return m_mapImage.getValueByIndex(uiIndex);
	}

	CResImage* CSkinParser::getImageByID(UINT uiID)
	{
		return m_mapImage.getValueByKey(uiID);
	}

	CResImage* CSkinParser::createImage()
	{
		CResImage *pImage = new CResImage;

		m_uiMaxImageID++;
		pImage->setID(m_uiMaxImageID);
		pImage->setType(L"IMAGE");

		m_mapImage.addItem(pImage->getID(), pImage);

		setDirty();

		return pImage;
	}

	void CSkinParser::deleteImage(CResImage *pImage)
	{
		if (pImage == NULL)
		{
			return;
		}

		m_mapImage.deleteItem(pImage->getID());

		setDirty();
	}

	UINT CSkinParser::getFontGroupSize()
	{
		return m_mapFontGroup.size();
	}

	CResFontGroup* CSkinParser::getFontGroupByIndex(UINT uiIndex)
	{
		return m_mapFontGroup.getValueByIndex(uiIndex);
	}

	CResFontGroup* CSkinParser::getFontGroupByID(UINT uiID)
	{
		return m_mapFontGroup.getValueByKey(uiID);
	}

	CResFontGroup* CSkinParser::createFontGroup()
	{
		CResFontGroup *pFontGroup = new CResFontGroup;

		m_uiMaxFontGroupID++;
		pFontGroup->setID(m_uiMaxFontGroupID);
		pFontGroup->setType(L"FONTGROUP");

		m_mapFontGroup.addItem(pFontGroup->getID(), pFontGroup);

		setDirty();

		return pFontGroup;
	}

	void CSkinParser::deleteFontGroup(CResFontGroup *pFontGroup)
	{
		if (pFontGroup == NULL)
		{
			return;
		}

		pFontGroup->deleteAllFont();
		m_mapFontGroup.deleteItem(pFontGroup->getID());

		setDirty();
	}

	UINT CSkinParser::getFontSize()
	{
		return m_mapFont.size();
	}

	CResFont* CSkinParser::getFontByIndex(UINT uiIndex)
	{
		return m_mapFont.getValueByIndex(uiIndex);
	}

	CResFont* CSkinParser::getFontByID(UINT uiID)
	{
		return m_mapFont.getValueByKey(uiID);
	}

	CResFont* CSkinParser::createFont()
	{
		CResFont *pFont = new CResFont;

		m_uiMaxFontID++;
		pFont->setID(m_uiMaxFontID);
		pFont->setType(L"FONT");

		m_mapFont.addItem(pFont->getID(), pFont);

		setDirty();

		return pFont;
	}

	void CSkinParser::deleteFont(CResFont *pFont)
	{
		if (pFont == NULL)
		{
			return;
		}

		m_mapFont.deleteItem(pFont->getID());

		setDirty();
	}

	UINT CSkinParser::getTextFieldSize()
	{
		return m_mapTextField.size();
	}

	CResTextField* CSkinParser::getTextFieldByIndex(UINT uiIndex)
	{
		return m_mapTextField.getValueByIndex(uiIndex);
	}

	CResTextField* CSkinParser::getTextFieldByID(UINT uiID)
	{
		return m_mapTextField.getValueByKey(uiID);
	}

	CResTextField* CSkinParser::createTextField()
	{
		CResTextField *pTextField = new CResTextField;

		m_uiMaxTextFieldID++;
		pTextField->setID(m_uiMaxTextFieldID);
		pTextField->setType(L"TEXTFIELD");

		m_mapTextField.addItem(pTextField->getID(), pTextField);

		setDirty();

		return pTextField;
	}

	void CSkinParser::deleteTextField(CResTextField *pTextField)
	{
		if (pTextField == NULL)
		{
			return;
		}

		m_mapTextField.deleteItem(pTextField->getID());

		setDirty();
	}

	UINT CSkinParser::getImageFieldSize()
	{
		return m_mapImageField.size();
	}

	CResImageField* CSkinParser::getImageFieldByIndex(UINT uiIndex)
	{
		return m_mapImageField.getValueByIndex(uiIndex);
	}

	CResImageField* CSkinParser::getImageFieldByID(UINT uiID)
	{
		return m_mapImageField.getValueByKey(uiID);
	}

	CResImageField* CSkinParser::createImageField()
	{
		CResImageField *pImageField = new CResImageField;

		m_uiMaxImageFieldID++;
		pImageField->setID(m_uiMaxImageFieldID);
		pImageField->setType(L"IMAGEFIELD");

		m_mapImageField.addItem(pImageField->getID(), pImageField);

		setDirty();

		return pImageField;
	}

	void CSkinParser::deleteImageField(CResImageField *pImageField)
	{
		if (pImageField == NULL)
		{
			return;
		}

		m_mapImageField.deleteItem(pImageField->getID());

		setDirty();
	}

	void CSkinParser::deleteIsolatedItem(CResItem  *pItem)
	{
		if (pItem == NULL || pItem->getParent() != NULL)
		{
			return;
		}

		pItem->deleteAllTextField();
		pItem->deleteAllImageField();

		delete pItem;
	}

	void CSkinParser::deleteIsolatedHitArea(CResHitArea *pHitArea)
	{
		if (pHitArea == NULL)
		{
			return;
		}

		delete pHitArea;
	}

	void CSkinParser::deleteIsolatedFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		delete pFilter;
	}

	void CSkinParser::setDirty()
	{
		m_bDirty = TRUE;
	}

	void CSkinParser::clearDirty()
	{
		m_bDirty = FALSE;
	}

	BOOLEAN CSkinParser::mergeScreen()
	{
		UINT uiScreenCount;

		uiScreenCount = m_pMergeSrc->getScreenSize();
		for (UINT uiIndex = 0; uiIndex < uiScreenCount; ++uiIndex)
		{
			CResScreen* pScreenSrc = m_pMergeSrc->getScreenByIndex(uiIndex);
			CResScreen* pScreenDest = this->getScreenByName(pScreenSrc->getName());

			if (pScreenDest == NULL)
			{
				pScreenDest = this->createScreen();
				pScreenDest->merge(pScreenSrc, TRUE);
			}
			else
			{
				pScreenDest->merge(pScreenSrc, FALSE);
			}
		}

		uiScreenCount = this->getScreenSize();
		for (INT iIndex = uiScreenCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResScreen* pScreenDest = this->getScreenByIndex(iIndex);
			CResScreen* pScreenSrc = m_pMergeSrc->getScreenByName(pScreenDest->getName());
			if (pScreenSrc == NULL)
			{
				this->deleteScreen(pScreenSrc);
			}
		}

		return TRUE;
	}

	BOOLEAN CSkinParser::mergePopup()
	{
		UINT uiPopupCount;

		uiPopupCount = m_pMergeSrc->getPopupSize();
		for (UINT uiIndex = 0; uiIndex < uiPopupCount; ++uiIndex)
		{
			CResPopup* pPopupSrc = m_pMergeSrc->getPopupByIndex(uiIndex);
			CResPopup* pPopupDest = this->getPopupByName(pPopupSrc->getName());

			if (pPopupDest == NULL)
			{
				pPopupDest = this->createPopup();
				pPopupDest->merge(pPopupSrc, TRUE);
			}
			else
			{
				pPopupDest->merge(pPopupSrc, FALSE);
			}
		}

		uiPopupCount = this->getPopupSize();
		for (INT iIndex = uiPopupCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResPopup* pPopupDest = this->getPopupByIndex(iIndex);
			CResPopup* pPopupSrc = m_pMergeSrc->getPopupByName(pPopupDest->getName());
			if (pPopupSrc == NULL)
			{
				this->deletePopup(pPopupSrc);
			}
		}

		return TRUE;	
	}

	BOOLEAN CSkinParser::mergeFuncWidget()
	{
		UINT uiFuncWidgetCount;

		uiFuncWidgetCount = m_pMergeSrc->getFuncWidgetSize();
		for (UINT uiIndex = 0; uiIndex < uiFuncWidgetCount; ++uiIndex)
		{
			CResFuncWidget* pFuncWidgetSrc = m_pMergeSrc->getFuncWidgetByIndex(uiIndex);
			CResFuncWidget* pFuncWidgetDest = this->getFuncWidgetByName(pFuncWidgetSrc->getName());

			if (pFuncWidgetDest == NULL)
			{
				pFuncWidgetDest = this->createFuncWidget();
				pFuncWidgetDest->merge(pFuncWidgetSrc, TRUE);
			}
			else
			{
				pFuncWidgetDest->merge(pFuncWidgetSrc, FALSE);
			}
		}

		uiFuncWidgetCount = this->getFuncWidgetSize();
		for (INT iIndex = uiFuncWidgetCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResFuncWidget* pFuncWidgetDest = this->getFuncWidgetByIndex(iIndex);
			CResFuncWidget* pFuncWidgetSrc = m_pMergeSrc->getFuncWidgetByName(pFuncWidgetDest->getName());
			if (pFuncWidgetSrc == NULL)
			{
				this->deleteFuncWidget(pFuncWidgetSrc);
			}
		}

		return TRUE;	
	}

	BOOLEAN CSkinParser::mergeCtrlWidget()
	{
		UINT uiCtrlWidgetCount;

		uiCtrlWidgetCount = m_pMergeSrc->getCtrlWidgetSize();
		for (UINT uiIndex = 0; uiIndex < uiCtrlWidgetCount; ++uiIndex)
		{
			CResCtrlWidget* pCtrlWidgetSrc = m_pMergeSrc->getCtrlWidgetByIndex(uiIndex);
			CResCtrlWidget* pCtrlWidgetDest = this->getCtrlWidgetByName(pCtrlWidgetSrc->getName());

			if (pCtrlWidgetDest == NULL)
			{
				pCtrlWidgetDest = this->createCtrlWidget();
				pCtrlWidgetDest->merge(pCtrlWidgetSrc, TRUE);
			}
			else
			{
				pCtrlWidgetDest->merge(pCtrlWidgetSrc, FALSE);
			}
		}

		uiCtrlWidgetCount = this->getCtrlWidgetSize();
		for (INT iIndex = uiCtrlWidgetCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResCtrlWidget* pCtrlWidgetDest = this->getCtrlWidgetByIndex(iIndex);
			CResCtrlWidget* pCtrlWidgetSrc = m_pMergeSrc->getCtrlWidgetByName(pCtrlWidgetDest->getName());
			if (pCtrlWidgetSrc == NULL)
			{
				this->deleteCtrlWidget(pCtrlWidgetSrc);
			}
		}

		return TRUE;
	}
	
	BOOLEAN CSkinParser::mergeTextField()
	{
		UINT uiTextFieldCount;

		uiTextFieldCount = m_pMergeSrc->getTextFieldSize();
		for (UINT uiIndex = 0; uiIndex < uiTextFieldCount; ++uiIndex)
		{
			CResTextField* pTextFieldSrc = m_pMergeSrc->getTextFieldByIndex(uiIndex);
			CResTextField* pTextFieldDest = this->getTextFieldByID(pTextFieldSrc->getID());

			if (pTextFieldDest == NULL)
			{
				pTextFieldDest = this->createTextField();
				pTextFieldDest->merge(pTextFieldSrc, TRUE);
			}
			else
			{
				pTextFieldDest->merge(pTextFieldSrc, FALSE);
			}
		}

		uiTextFieldCount = this->getTextFieldSize();
		for (INT iIndex = uiTextFieldCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResTextField* pTextFieldDest = this->getTextFieldByIndex(iIndex);
			CResTextField* pTextFieldSrc = m_pMergeSrc->getTextFieldByID(pTextFieldDest->getID());
			if (pTextFieldSrc == NULL)
			{
				this->deleteTextField(pTextFieldSrc);
			}
		}

		return TRUE;
	}

	BOOLEAN CSkinParser::mergeImageField()
	{
		UINT uiImageFieldCount;

		uiImageFieldCount = m_pMergeSrc->getImageFieldSize();
		for (UINT uiIndex = 0; uiIndex < uiImageFieldCount; ++uiIndex)
		{
			CResImageField* pImageFieldSrc = m_pMergeSrc->getImageFieldByIndex(uiIndex);
			CResImageField* pImageFieldDest = this->getImageFieldByID(pImageFieldSrc->getID());

			if (pImageFieldDest == NULL)
			{
				pImageFieldDest = this->createImageField();
				pImageFieldDest->merge(pImageFieldSrc, TRUE);
			}
			else
			{
				pImageFieldDest->merge(pImageFieldSrc, FALSE);
			}
		}

		uiImageFieldCount = this->getImageFieldSize();
		for (INT iIndex = uiImageFieldCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResImageField* pImageFieldDest = this->getImageFieldByIndex(iIndex);
			CResImageField* pImageFieldSrc = m_pMergeSrc->getImageFieldByID(pImageFieldDest->getID());
			if (pImageFieldSrc == NULL)
			{
				this->deleteImageField(pImageFieldSrc);
			}
		}

		return TRUE;
	}

	void CSkinParser::buildScreenData(CNodeParser &parser)
	{
		m_mapScreen.clear();

		for (UINT uiScreenIndex = 0; uiScreenIndex < parser.getSectionCount(); uiScreenIndex++)
		{
			String strKey = String::format(L"SCREEN_%d", uiScreenIndex + 1);
			Map<String, String, tagStringCompare> *pSection = parser.getSection(strKey);

			if (pSection != NULL)
			{
				CResScreen *pScreen = new CResScreen;
				pScreen->parse(pSection);
				m_mapScreen.addItem(pScreen->getPropertyInt(L"id"), pScreen);
			}
			else
			{
				break;
			}
		}
	}

	void CSkinParser::buildPopupData(CNodeParser &parser)
	{
		m_mapPopup.clear();

		for (UINT uiPopupIndex = 0; uiPopupIndex < parser.getSectionCount(); uiPopupIndex++)
		{
			String strKey = String::format(L"POPUP_%d", uiPopupIndex + 1);
			Map<String, String, tagStringCompare> *pSection = parser.getSection(strKey);

			if (pSection != NULL)
			{
				CResPopup *pPopup = new CResPopup;
				pPopup->parse(pSection);
				m_mapPopup.addItem(pPopup->getPropertyInt(L"id"), pPopup);
			}
			else
			{
				break;
			}
		}
	}

	void CSkinParser::buildFuncWidgetData(CNodeParser &parser)
	{
		m_mapFuncWidget.clear();

		for (UINT uiFuncIndex = 0; uiFuncIndex < parser.getSectionCount(); uiFuncIndex++)
		{
			String strKey = String::format(L"FUNCWIDGET_%d", uiFuncIndex + 1);
			Map<String, String, tagStringCompare> *pSection = parser.getSection(strKey);

			if (pSection != NULL )
			{
				CResFuncWidget *pFunc = new CResFuncWidget;
				pFunc->parse(pSection);
				m_mapFuncWidget.addItem(pFunc->getPropertyInt(L"id"), pFunc);
			}
			else
			{
				break;
			}
		}
	}

	void CSkinParser::buildCtrlWidgetData(CNodeParser &parser)
	{
		m_mapCtrlWidget.clear();

		for (UINT uiCtrlIndex = 0; uiCtrlIndex < parser.getSectionCount(); uiCtrlIndex++)
		{
			String strKey = String::format(L"CTRLWIDGET_%d", uiCtrlIndex + 1);
			Map<String, String, tagStringCompare> *pSection = parser.getSection(strKey);

			if (pSection != NULL )
			{
				CResCtrlWidget *pCtrl = new CResCtrlWidget;
				pCtrl->parse(pSection);
				m_mapCtrlWidget.addItem(pCtrl->getPropertyInt(L"id"), pCtrl);
			}
			else
			{
				break;
			}
		}
	}

	void CSkinParser::buildImageData(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare> *pImageSection = parser.getSection(L"IMAGE");

		if (pImageSection == NULL)
		{
			return;
		}

		m_mapImage.clear();

		UINT uiSize = pImageSection->size();

		for (UINT uiImageIndex = 0; uiImageIndex < uiSize; uiImageIndex++)
		{
			String strKey = String::format(L"img%d",uiImageIndex + 1);
			String *pstrValue = pImageSection->getValueByKey(strKey);
			if (pstrValue != NULL && *pstrValue != STRING_NULL)
			{
				CResImage *pImage = new CResImage;
				pImage->parseProperty(*pstrValue);
				m_mapImage.addItem(pImage->getPropertyInt(L"id"), pImage);
			}
		}
	}

	void CSkinParser::buildFontGroupData(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare> *pFontGroupSection = parser.getSection(L"FONTGROUP");

		if (pFontGroupSection == NULL)
		{
			return;
		}

		m_mapFontGroup.clear();

		UINT uiSize = pFontGroupSection->size();

		for (UINT uiFontGroupIndex = 0; uiFontGroupIndex < uiSize; uiFontGroupIndex++)
		{
			String strKey = String::format(L"fntgroup%d", uiFontGroupIndex + 1);
			String *pstrValue = pFontGroupSection->getValueByKey(strKey);
			if (pstrValue != NULL && *pstrValue != STRING_NULL)
			{
				CResFontGroup *pFontGroup = new CResFontGroup;
				pFontGroup->parseProperty(*pstrValue);
				m_mapFontGroup.addItem(pFontGroup->getPropertyInt(L"id"), pFontGroup);
			}
		}
	}

	void CSkinParser::buildFontData(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare> *pFontSection = parser.getSection(L"FONT");

		if (pFontSection == NULL)
		{
			return;
		}

		m_mapFont.clear();

		UINT uiSize = pFontSection->size();

		for (UINT uiFontIndex = 0; uiFontIndex < uiSize; uiFontIndex++)
		{
			String strKey = String::format(L"font%d",uiFontIndex + 1);
			String *pstrValue = pFontSection->getValueByKey(strKey);
			if (pstrValue != NULL && *pstrValue != STRING_NULL)
			{
				CResFont *pFont = new CResFont;
				pFont->parseProperty(*pstrValue);
				m_mapFont.addItem(pFont->getPropertyInt(L"id"), pFont);
			}
		}
	}

	void CSkinParser::buildTextFieldData(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare> *pTextFieldSection = parser.getSection(L"TEXTFIELD");

		if (pTextFieldSection == NULL)
		{
			return;
		}
		m_mapTextField.clear();

		UINT uiSize = pTextFieldSection->size();
		for (UINT uiTextIndex = 0; uiTextIndex < uiSize; uiTextIndex++)
		{
			String strKey = String::format(L"txtfield%d",uiTextIndex + 1);
			String *pstrValue = pTextFieldSection->getValueByKey(strKey);
			if (pstrValue != NULL && *pstrValue != STRING_NULL)
			{
				CResTextField *pTextField = new CResTextField;
				pTextField->parseProperty(*pstrValue);
				m_mapTextField.addItem(pTextField->getPropertyInt(L"id"), pTextField);
			}
		}
	}

	void CSkinParser::buildImageFieldData(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare> *pImageFieldSection = parser.getSection(L"IMAGEFIELD");

		if (pImageFieldSection == NULL)
		{
			return;
		}
		m_mapImageField.clear();

		UINT uiSize = pImageFieldSection->size();
		for (UINT uiImageIndex = 0; uiImageIndex < uiSize; uiImageIndex++)
		{
			String strKey = String::format(L"imgfield%d",uiImageIndex + 1);
			String *pstrValue = pImageFieldSection->getValueByKey(strKey);
			if (pstrValue != NULL && *pstrValue != STRING_NULL)
			{
				CResImageField *pImageField = new CResImageField;
				pImageField->parseProperty(*pstrValue);
				m_mapImageField.addItem(pImageField->getPropertyInt(L"id"), pImageField);
			}
		}
	}

	void CSkinParser::buildGlobalData(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare> *pGlobalSection = parser.getSection(L"GLOBAL");

		if (pGlobalSection == NULL)
		{
			return;
		}

		String *pstrValue = pGlobalSection->getValueByKey(L"skinName");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_strSkinName = *pstrValue;
		}
		
		pstrValue = pGlobalSection->getValueByKey(L"width");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiScreenWidth = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"height");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiScreenHeight = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxScreenID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxScreenID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxPopupID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxPopupID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxFuncWidgetID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxFuncWidgetID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxCtrlWidgetID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxCtrlWidgetID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxImageID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxImageID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxFontGroupID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxFontGroupID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxFontID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxFontID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxTextFieldID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxTextFieldID = (UINT)String::toUInt(*pstrValue);
		}

		pstrValue = pGlobalSection->getValueByKey(L"maxImageFieldID");
		if (pstrValue != NULL && *pstrValue != STRING_NULL)
		{
			m_uiMaxImageFieldID = (UINT)String::toUInt(*pstrValue);
		}
	}

	void CSkinParser::saveScreen(CNodeParser &parser)
	{
		String	strSection;
		UINT	uiIndex = 1;
		
		m_mapScreen.resetIterator();
		while (m_mapScreen.hasMore())
		{
			strSection = String::format(L"SCREEN_%d", uiIndex++);
			parser.createSection(strSection);

			CResScreen *pScreen = m_mapScreen.next();
	
			parser.setItem(strSection, L"base", pScreen->saveProperty()); //base part
			Map<String, String, tagStringCompare> mapData;
			pScreen->save(&mapData);

			mapData.resetIterator();
			while (mapData.hasMore())
			{
				MapPair<String, String> itemPair = mapData.nextKeyAndValue();
				parser.setItem(strSection, *itemPair.pKey, *itemPair.pData);
			}
		}
	}

	void CSkinParser::savePopup(CNodeParser &parser)
	{
		String	strSection;
		UINT	uiIndex = 1;
		
		m_mapPopup.resetIterator();
		while (m_mapPopup.hasMore())
		{
			strSection = String::format(L"POPUP_%d", uiIndex++);
			parser.createSection(strSection);

			CResPopup *pPopup = m_mapPopup.next();
	
			parser.setItem(strSection, L"base", pPopup->saveProperty()); //base part
			Map<String, String, tagStringCompare> mapData;
			pPopup->save(&mapData);

			mapData.resetIterator();
			while (mapData.hasMore())
			{
				MapPair<String, String> itemPair = mapData.nextKeyAndValue();
				parser.setItem(strSection, *itemPair.pKey, *itemPair.pData);
			}
		}
	}

	void CSkinParser::saveFuncWidget(CNodeParser &parser)
	{
		String	strSection;
		UINT	uiIndex = 1;
		
		m_mapFuncWidget.resetIterator();
		while (m_mapFuncWidget.hasMore())
		{
			strSection = String::format(L"FUNCWIDGET_%d", uiIndex++);
			parser.createSection(strSection);

			CResFuncWidget *pFun = m_mapFuncWidget.next();
	
			parser.setItem(strSection, L"base", pFun->saveProperty()); //base part
			Map<String, String, tagStringCompare> mapData;
			pFun->save(&mapData);

			mapData.resetIterator();
			while (mapData.hasMore())
			{
				MapPair<String, String> itemPair = mapData.nextKeyAndValue();
				parser.setItem(strSection, *itemPair.pKey, *itemPair.pData);
			}
		}
	}

	void CSkinParser::saveCtrlWidget(CNodeParser &parser)
	{
		String	strSection;
		UINT	uiIndex = 1;
		
		m_mapCtrlWidget.resetIterator();
		while (m_mapCtrlWidget.hasMore())
		{
			strSection = String::format(L"CTRLWIDGET_%d", uiIndex++);
			parser.createSection(strSection);

			CResCtrlWidget *pCtrl = m_mapCtrlWidget.next();
	
			parser.setItem(strSection, L"base", pCtrl->saveProperty()); //base part
			Map<String, String, tagStringCompare> mapData;
			pCtrl->save(&mapData);

			mapData.resetIterator();
			while (mapData.hasMore())
			{
				MapPair<String, String> itemPair = mapData.nextKeyAndValue();
				parser.setItem(strSection, *itemPair.pKey, *itemPair.pData);
			}
		}
	}

	void CSkinParser::saveImage(CNodeParser &parser)
	{
		String	strKey;
		UINT	uiIndex = 1;
		
		parser.createSection(L"IMAGE");

		m_mapImage.resetIterator();
		while (m_mapImage.hasMore())
		{
			strKey = String::format(L"img%d", uiIndex++);

			CResImage *pImage = m_mapImage.next();

			parser.setItem(L"IMAGE", strKey, pImage->saveProperty());
		}
	}

	void CSkinParser::saveFontGroup(CNodeParser &parser)
	{
		String	strKey;
		UINT	uiIndex = 1;

		parser.createSection(L"FONTGROUP");

		m_mapFontGroup.resetIterator();
		while (m_mapFontGroup.hasMore())
		{
			strKey = String::format(L"fntgroup%d", uiIndex++);

			CResFontGroup *pFontGroup = m_mapFontGroup.next();

			parser.setItem(L"FONTGROUP", strKey, pFontGroup->saveProperty());
		}
	}

	void CSkinParser::saveFont(CNodeParser &parser)
	{
		String	strKey;
		UINT	uiIndex = 1;
		
		parser.createSection(L"FONT");

		m_mapFont.resetIterator();
		while (m_mapFont.hasMore())
		{
			strKey = String::format(L"font%d", uiIndex++);

			CResFont *pFont = m_mapFont.next();

			parser.setItem(L"FONT", strKey, pFont->saveProperty());
		}
	}

	void CSkinParser::saveTextField(CNodeParser &parser)
	{
		String	strKey;
		UINT	uiIndex = 1;
		
		parser.createSection(L"TEXTFIELD");

		m_mapTextField.resetIterator();
		while (m_mapTextField.hasMore())
		{
			strKey = String::format(L"txtfield%d", uiIndex++);

			CResTextField *pTextField = m_mapTextField.next();

			parser.setItem(L"TEXTFIELD", strKey, pTextField->saveProperty());
		}
	}

	void CSkinParser::saveImageField(CNodeParser &parser)
	{
		String	strKey;
		UINT	uiIndex = 1;
		
		parser.createSection(L"IMAGEFIELD");

		m_mapImageField.resetIterator();
		while (m_mapImageField.hasMore())
		{
			strKey = String::format(L"imgfield%d", uiIndex++);

			CResImageField *pImageField = m_mapImageField.next();

			parser.setItem(L"IMAGEFIELD", strKey, pImageField->saveProperty());
		}
	}

	void CSkinParser::saveGlobal(CNodeParser &parser)
	{
		parser.createSection(L"GLOBAL");
		parser.setItem(L"GLOBAL", L"skinName", m_strSkinName);
		parser.setItem(L"GLOBAL", L"width", m_uiScreenWidth);
		parser.setItem(L"GLOBAL", L"height", m_uiScreenHeight);
		parser.setItem(L"GLOBAL", L"maxScreenID", m_uiMaxScreenID);
		parser.setItem(L"GLOBAL", L"maxPopupID", m_uiMaxPopupID);
		parser.setItem(L"GLOBAL", L"maxFuncWidgetID", m_uiMaxFuncWidgetID);
		parser.setItem(L"GLOBAL", L"maxCtrlWidgetID", m_uiMaxCtrlWidgetID);
		parser.setItem(L"GLOBAL", L"maxImageID", m_uiMaxImageID);
		parser.setItem(L"GLOBAL", L"maxFontGroupID", m_uiMaxFontGroupID);
		parser.setItem(L"GLOBAL", L"maxFontID", m_uiMaxFontID);
		parser.setItem(L"GLOBAL", L"maxTextFieldID", m_uiMaxTextFieldID);
		parser.setItem(L"GLOBAL", L"maxImageFieldID", m_uiMaxImageFieldID);
	}

	void CSkinParser::initDefaultValue()
	{
		m_strSkinName = DEFAULT_SKIN_NAME;
		m_uiScreenWidth = DEFAULT_SCREEN_WIDTH;
		m_uiScreenHeight = DEFAULT_SCREEN_HEIGHT;

		m_uiMaxScreenID = 0;
		m_uiMaxPopupID = 0;
		m_uiMaxFuncWidgetID = 0;
		m_uiMaxCtrlWidgetID = 0;
		m_uiMaxImageID = 0;
		m_uiMaxFontGroupID = 0;
		m_uiMaxFontID = 0;
		m_uiMaxTextFieldID = 0;
		m_uiMaxImageFieldID = 0;
	}
}