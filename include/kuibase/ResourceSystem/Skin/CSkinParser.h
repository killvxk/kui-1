/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CSkinParser
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method
		static	CSkinParser*	getInstance();
		static	void			deleteInstance();

		BOOLEAN			createSkin(const String &strPath);
		void			closeSkin();
		BOOLEAN			hasLoadSkin();
		BOOLEAN			parse(const String &strPath);
		BOOLEAN			save(const String &strPath);		
		BOOLEAN			merge(const String &strSkinPath);
		
		const String&   getSkinPath() const;
		const String&	getSkinFolder() const;
		const String&	getPrivateImageFolder() const;
		const String&	getCommonImageFolder();

		const String&	getSkinName() const;
		void			setSkinName(const String strValue);

		UINT			getScreenWidth() const;
		void			setScreenWidth(UINT iWidth);
		UINT			getScreenHeight() const;
		void			setScreenHeight(UINT iHeight);

		UINT			getScreenSize();
		CResScreen*		getScreenByIndex(UINT uiIndex);
		CResScreen*		getScreenByID(UINT uiID);
		CResScreen*		getScreenByName(const String &strName);
		CResScreen*		createScreen();
		void			deleteScreen(CResScreen *pScreen);

		UINT			getPopupSize();
		CResPopup*		getPopupByIndex(UINT uiIndex);
		CResPopup*		getPopupByName(const String &strName);
		CResPopup*		getPopupByID(UINT uiID);		
		CResPopup*		createPopup();
		void			deletePopup(CResPopup *pPopup);

		UINT			getFuncWidgetSize();
		CResFuncWidget* getFuncWidgetByIndex(UINT uiIndex);
		CResFuncWidget*	getFuncWidgetByID(UINT uiID);		
		CResFuncWidget*	getFuncWidgetByName(const String &strName);
		CResFuncWidget*	createFuncWidget();
		void			deleteFuncWidget(CResFuncWidget *pFuncWidget);

		UINT			getCtrlWidgetSize();
		CResCtrlWidget* getCtrlWidgetByIndex(UINT uiIndex);
		CResCtrlWidget*	getCtrlWidgetByID(UINT uiID);		
		CResCtrlWidget*	getCtrlWidgetByName(const String &strName);
		CResCtrlWidget*	createCtrlWidget();
		void			deleteCtrlWidget(CResCtrlWidget *pCtrlWidget);

		UINT			getImageSize();
		CResImage*		getImageByIndex(UINT uiIndex);
		CResImage*		getImageByID(UINT uiID);
		CResImage*		createImage();
		void			deleteImage(CResImage *pImage);	

		UINT			getFontGroupSize();
		CResFontGroup*	getFontGroupByIndex(UINT uiIndex);
		CResFontGroup*	getFontGroupByID(UINT uiID);
		CResFontGroup*	createFontGroup();
		void			deleteFontGroup(CResFontGroup *pFontGroup);		

		//----start---inner methods
		UINT			getFontSize();
		CResFont*		getFontByIndex(UINT uiIndex);
		CResFont*		getFontByID(UINT uiID);
		CResFont*		createFont();
		void			deleteFont(CResFont *pFont);
		
		UINT			getTextFieldSize();
		CResTextField*	getTextFieldByIndex(UINT uiIndex);
		CResTextField*	getTextFieldByID(UINT uiID);
		CResTextField*	createTextField();
		void			deleteTextField(CResTextField *pText);	

		UINT			getImageFieldSize();
		CResImageField*	getImageFieldByIndex(UINT uiIndex);
		CResImageField*	getImageFieldByID(UINT uiID);
		CResImageField*	createImageField();
		void			deleteImageField(CResImageField *pText);
		//----end---inner methods

		void			deleteIsolatedItem(CResItem  *pItem);
		void			deleteIsolatedHitArea(CResHitArea *pHitArea);
		void			deleteIsolatedFilter(CResFilter *pFilter);

		BOOLEAN			isDirty()								{ return m_bDirty;	}
		void			setDirty();
		void			clearDirty();

	protected:// method
		CSkinParser();
		virtual ~CSkinParser();

	private:// method
		void			buildScreenData(CNodeParser &parser);
		void			buildPopupData(CNodeParser &parser);
		void			buildFuncWidgetData(CNodeParser &parser);
		void			buildCtrlWidgetData(CNodeParser &parser);
		void			buildImageData(CNodeParser &parser);
		void			buildFontGroupData(CNodeParser &parser);
		void			buildFontData(CNodeParser &parser);
		void			buildTextFieldData(CNodeParser &parser);
		void			buildImageFieldData(CNodeParser &parser);
		void			buildGlobalData(CNodeParser &parser);

		void			saveScreen(CNodeParser &parser);
		void			savePopup(CNodeParser &parser);
		void			saveFuncWidget(CNodeParser &parser);
		void			saveCtrlWidget(CNodeParser &parser);
		void			saveImage(CNodeParser &parser);
		void			saveFontGroup(CNodeParser &parser);
		void			saveFont(CNodeParser &parser);
		void			saveTextField(CNodeParser &parser);
		void			saveImageField(CNodeParser &parser);
		void			saveGlobal(CNodeParser &parser);

		void			initDefaultValue();

		BOOLEAN			mergeScreen();
		BOOLEAN			mergePopup();
		BOOLEAN			mergeFuncWidget();
		BOOLEAN			mergeCtrlWidget();

		BOOLEAN			mergeTextField();
		BOOLEAN			mergeImageField();

	protected:// property

	private:// property
		static	CSkinParser	*s_pInstance;

		String											m_strSkinPath;
		String											m_strSkinFolder;
		String											m_strPrivateImgFolder;
		String											m_strCommonImgFolder;

		String											m_strSkinName;
		UINT											m_uiScreenWidth;
		UINT											m_uiScreenHeight;

		UINT											m_uiMaxScreenID;
		UINT											m_uiMaxPopupID;
		UINT											m_uiMaxFuncWidgetID;
		UINT											m_uiMaxCtrlWidgetID;
		UINT											m_uiMaxImageID;
		UINT											m_uiMaxFontGroupID;
		UINT											m_uiMaxFontID;
		UINT											m_uiMaxTextFieldID;
		UINT											m_uiMaxImageFieldID;

		// outer resource
		Map<UINT, CResScreen, tagNumCompare<UINT> >		m_mapScreen;
		Map<UINT, CResPopup, tagNumCompare<UINT> >		m_mapPopup;
		Map<UINT, CResFuncWidget, tagNumCompare<UINT> >	m_mapFuncWidget;
		Map<UINT, CResCtrlWidget, tagNumCompare<UINT> >	m_mapCtrlWidget;
		Map<UINT, CResImage, tagNumCompare<UINT> >		m_mapImage;		
		Map<UINT, CResFontGroup, tagNumCompare<UINT> >	m_mapFontGroup;	

		// inner resource
		Map<UINT, CResFont, tagNumCompare<UINT> >		m_mapFont;
		Map<UINT, CResTextField, tagNumCompare<UINT> >	m_mapTextField;
		Map<UINT, CResImageField, tagNumCompare<UINT> >	m_mapImageField;

		BOOLEAN											m_bDirty;

		CSkinParser*									m_pMergeSrc;
	};
}