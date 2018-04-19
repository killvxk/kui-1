#include "OpenHMI.h"

#define RETURN_IF_INITED_FROM_RESOURCE(targetParent) \
if ((targetParent)->getHasInitFromResource()) \
{ \
	return; \
} \
else \
{ \
	(targetParent)->setHasInitFromResource(TRUE); \
} \

namespace OpenHMI
{	
	void INIT::initMainFrame(CBaseMainFrame *pMainFrame, UINT uiID)
	{
		if (pMainFrame == NULL)
		{
			return;
		}		

		CResScreen *pResScreen = CSkinParser::getInstance()->getScreenByID(uiID);
		if (pResScreen == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(pMainFrame)

		pMainFrame->setID(uiID);		
		pMainFrame->setSkin(pResScreen->getPropertyUInt(L"background"));

		initFilter(*pMainFrame, pResScreen);
	}

	void INIT::initScreen(CBaseScreen *pScreen, UINT uiID)
	{
		if (pScreen == NULL)
		{
			return;
		}		

		CResScreen *pResScreen = CSkinParser::getInstance()->getScreenByID(uiID);
		if (pResScreen == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(pScreen)

		pScreen->setID(uiID);		
		pScreen->setSkin(pResScreen->getPropertyUInt(L"background"));

		initFilter(*pScreen, pResScreen);
	}

	void INIT::initPopup(CBasePopup *pPopup, UINT uiID)
	{
		if (pPopup == NULL)
		{
			return;
		}		

		CResPopup *pResPopup = CSkinParser::getInstance()->getPopupByID(uiID);
		if (pResPopup == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(pPopup)

		pPopup->setID(uiID);		
		pPopup->setSkin(pResPopup->getPropertyUInt(L"background"));
		pPopup->setRealX(pResPopup->getPropertyUInt(L"realx"));
		pPopup->setRealY(pResPopup->getPropertyUInt(L"realy"));
		pPopup->setRealWidth(pResPopup->getPropertyUInt(L"realwidth"));
		pPopup->setRealHeight(pResPopup->getPropertyUInt(L"realheight"));

		initFilter(*pPopup, pResPopup);
	}

	void INIT::initMessageBox(CWgtMessageBox *pMessageBox, UINT uiID)
	{
		if (pMessageBox == NULL)
		{
			return;
		}		

		CResCtrlWidget *pResCtrlWidget = CSkinParser::getInstance()->getCtrlWidgetByID(uiID);
		if (pResCtrlWidget == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(pMessageBox)

		pMessageBox->setID(uiID);
		pMessageBox->setSkin(pResCtrlWidget->getPropertyUInt(L"background"));
		pMessageBox->setRealWidth(pResCtrlWidget->getPropertyUInt(L"width"));
		pMessageBox->setRealHeight(pResCtrlWidget->getPropertyUInt(L"height"));

		initFilter(*pMessageBox, pResCtrlWidget);

		for (UINT i = 0; i < pResCtrlWidget->getItemSize(); i++)
		{
			CResItem *pResItem = pResCtrlWidget->getItemByIndex(i);

			const String& strType = pResItem->getType();
			const String& strName = pResItem->getName();

			if (strType == L"BUTTON")
			{
				if (strName == L"mOk")
				{
					initButton(*(pMessageBox->getButtonOK()), pResItem);					
				}
				else if (strName == L"mCancel")
				{
					initButton(*(pMessageBox->getButtonCancel()), pResItem);
				}
				else if (strName == L"mAlternate")
				{
					initButton(*(pMessageBox->getButtonAlternate()), pResItem);
				}
			}
			else if (strType == L"TEXTFIELD")
			{
				if (strName == L"mTitle")
				{
					CCtrlTextField *pTxtField = pMessageBox->getTitleTextField();
					initTextField( *pTxtField, pResItem);
					initFilter(*pTxtField, pResItem);
				}
				else
				{
					CCtrlTextField *pTxtField = pMessageBox->extractTextField(strName);
					initTextField( *pTxtField, pResItem);
					initFilter(*pTxtField, pResItem);
				}
			}
			else if (strType == L"IMAGEFIELD")
			{
				CCtrlImageField *pImgField = pMessageBox->extractImageField(strName);
				initImageField(	*pImgField,  pResItem);
				initFilter(*pImgField, pResItem);
			}
		}
	}	

	void INIT::initFuncWidget(CBaseFuncWidget *pFuncWidget, UINT uiID)
	{
		if (pFuncWidget == NULL)
		{
			return;
		}		

		CResFuncWidget *pResFuncWidget = CSkinParser::getInstance()->getFuncWidgetByID(uiID);
		if (pResFuncWidget == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(pFuncWidget)

		pFuncWidget->setID(uiID);
		pFuncWidget->setSkin(pResFuncWidget->getPropertyUInt(L"background"));
		pFuncWidget->setPreferredSize(pResFuncWidget->getPropertyUInt(L"width"), pResFuncWidget->getPropertyUInt(L"height"));

		initFilter(*pFuncWidget, pResFuncWidget);
	}

	void INIT::initFuncWidget(CBaseFuncWidget &pFuncWidget, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		pFuncWidget.setVisible(pResItem->getPropertyBool(L"visible"));
		pFuncWidget.setX(pResItem->getPropertyInt(L"x"));
		pFuncWidget.setY(pResItem->getPropertyInt(L"y"));

		initHitTestArea(pFuncWidget, pResItem);
		initFilter(pFuncWidget, pResItem);
	}

	void INIT::initKeyboard(CWgtKeyboard &pKeyboard, UINT uiID, CContainer* pParent)
	{
		// Keyboard in screen -- ID is from screen
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pKeyboard)

		pKeyboard.setVisible(pResItem->getPropertyBool(L"visible"));
		pKeyboard.setX(pResItem->getPropertyInt(L"x"));
		pKeyboard.setY(pResItem->getPropertyInt(L"y"));

		// Keyboard itself,  refID is from skin.ini,  globle ID
		UINT uiRefID = pResItem->getPropertyUInt(L"refID");
		pKeyboard.setID(uiRefID);

		CResCtrlWidget *pResCtrlWidget = CSkinParser::getInstance()->getCtrlWidgetByID(uiRefID);
		if (pResCtrlWidget == NULL)
		{
			return;
		}

		pKeyboard.setPreferredSize(pResCtrlWidget->getPropertyUInt(L"width"), pResCtrlWidget->getPropertyUInt(L"height"));
		pKeyboard.setSkin(pResCtrlWidget->getPropertyUInt(L"background"));

		initHitTestArea(pKeyboard, pResItem);
		initFilter(pKeyboard, pResItem);

		for (UINT i = 0; i < pResCtrlWidget->getItemSize(); i++)
		{
			CResItem *pResItem = pResCtrlWidget->getItemByIndex(i);

			const String& strType = pResItem->getType();
			const String& strName = pResItem->getName();

			if (strType == L"BUTTON")
			{
				initButton(*(pKeyboard.getButton(strName)), pResItem);
			}
		}
	}

	void INIT::initSelector(CCtrlSelector &pSelector, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pSelector)

		pSelector.setVisible(pResItem->getPropertyBool(L"visible"));
		pSelector.setX(pResItem->getPropertyInt(L"x"));
		pSelector.setY(pResItem->getPropertyInt(L"y"));
		pSelector.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pSelector.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));

		String strLayout = pResItem->getPropertyString(L"layout");
		if (strLayout == L"landscape")
		{
			pSelector.setLayout(CCtrlSelector::LT_LANDSCRAPE);
		}
		else if (strLayout == L"portrait")
		{
			pSelector.setLayout(CCtrlSelector::LT_PROTRAIT);
		}

		// Selector itself,  refID is from skin.ini,  globle ID
		UINT uiRefID = pResItem->getPropertyUInt(L"refID");
		pSelector.setID(uiRefID);

		CResCtrlWidget *pResCtrlWidget = CSkinParser::getInstance()->getCtrlWidgetByID(uiRefID);
		if (pResCtrlWidget == NULL)
		{
			return;
		}

		pSelector.setPreferredSize(pResCtrlWidget->getPropertyUInt(L"width"), pResCtrlWidget->getPropertyUInt(L"height"));
		pSelector.setSkin(pResCtrlWidget->getPropertyUInt(L"background"));

		initHitTestArea(pSelector, pResItem);
		initFilter(pSelector, pResItem);

		for (UINT i = 0; i < pResCtrlWidget->getItemSize(); i++)
		{
			CResItem *pResItem = pResCtrlWidget->getItemByIndex(i);

			const String& strType = pResItem->getType();
			const String& strName = pResItem->getName();

			if (strType == L"BUTTON")
			{
				initButton(*(pSelector.getButton(strName)), pResItem);
			}
		}
	}


	void INIT::initButton(CCtrlButton &pButton, UINT uiID, CContainer *pParent)
	{
		initButton(pButton, findResItem(pParent, uiID));
	}

	void INIT::initLabel(CCtrlLabel &pLabel, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}		
		
		RETURN_IF_INITED_FROM_RESOURCE(&pLabel)

		pLabel.setID(uiID);
		pLabel.setVisible(pResItem->getPropertyBool(L"visible"));
		pLabel.setX(pResItem->getPropertyInt(L"x"));
		pLabel.setY(pResItem->getPropertyInt(L"y"));
		pLabel.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pLabel.setSkin(pResItem->getPropertyUInt(L"background"));
		pLabel.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));

		initHitTestArea(pLabel, pResItem);
		initFilter(pLabel, pResItem);

		UINT ui = 0;
		for( ui=0; ui < pResItem->getTextFieldSize(); ui++ )
		{
			CResTextField *pResTextField = pResItem->getTextFieldByIndex(ui);

			initTextField(*(pLabel.extractTextField(pResTextField->getName())), pResTextField);
		}

		for( ui=0; ui < pResItem->getImageFieldSize(); ui++ )
		{
			CResImageField *pResImageField = pResItem->getImageFieldByIndex(ui);

			initImageField(*(pLabel.extractImageField(pResImageField->getName())), pResImageField);
		}
	}

	void INIT::initTextField(CCtrlTextField &pTextField, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);

		initTextField(pTextField, pResItem);

		initFilter(pTextField, pResItem);
	}

	void INIT::initImageField(CCtrlImageField &pImageField, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);

		initImageField(pImageField, pResItem);

		initFilter(pImageField, pResItem);
	}

	void INIT::initWebBrowser(CCtrlWebBrowser &pWebBrowser, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);

		initWebBrowser(pWebBrowser, pResItem);
	}

	void INIT::initEdit(CCtrlEdit &pEdit, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);

		initEdit(pEdit, pResItem);
	}

	void INIT::initViewport(CCtrlViewport &pViewport, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}
		
		RETURN_IF_INITED_FROM_RESOURCE(&pViewport)

		pViewport.setID(uiID);
		pViewport.setVisible(pResItem->getPropertyBool(L"visible"));
		pViewport.setX(pResItem->getPropertyInt(L"x"));
		pViewport.setY(pResItem->getPropertyInt(L"y"));
		pViewport.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		

		String strStyleType = pResItem->getPropertyString(L"style");
		CPropertySet *pResPropertySet = pResItem->getPropertySet(L"style_param");
		if (strStyleType == L"normal")
		{
			CRenderViewport::CStyleNormal *pStyle = new CRenderViewport::CStyleNormal();

			pViewport.setStyle(pStyle);
		}
		else if (strStyleType == L"transparent")
		{
			CRenderViewport::CStyleTransparent *pStyle = new CRenderViewport::CStyleTransparent();

			pViewport.setStyle(pStyle);
		}
		else if (strStyleType == L"capture")
		{
			CRenderViewport::CStyleCapture *pStyle = new CRenderViewport::CStyleCapture();

			pViewport.setStyle(pStyle);
		}
		else if (strStyleType == L"waver")
		{
			CRenderViewport::CStyleWaver *pStyle = new CRenderViewport::CStyleWaver();
			if (pResPropertySet != NULL)
			{
				pStyle->uiWidth = pResPropertySet->getPropertyUInt(L"width");
				pStyle->uiHeight = pResPropertySet->getPropertyUInt(L"height");
				pStyle->uiRow = pResPropertySet->getPropertyUInt(L"row");
				pStyle->uiColumn = pResPropertySet->getPropertyUInt(L"column");
				ObjectPtr<CMovieClip> pMovieClip( ENV::getMovieClip(pResPropertySet->getPropertyUInt(L"textureimg")) );
				if (pMovieClip != NULL)
				{
					pStyle->strTextureImage = pMovieClip->getCurrentImage();
				}				
			}			

			pViewport.setStyle(pStyle);
		}
		else
		{
			CRenderViewport::CStyleNormal *pStyle = new CRenderViewport::CStyleNormal();

			pViewport.setStyle(pStyle);
		}

		initHitTestArea(pViewport, pResItem);
		initFilter(pViewport, pResItem);
	}	

	void INIT::initBalance(CCtrlBalance &pBalance, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}		

		RETURN_IF_INITED_FROM_RESOURCE(&pBalance)

		pBalance.setID(uiID);
		pBalance.setVisible(pResItem->getPropertyBool(L"visible"));
		pBalance.setX(pResItem->getPropertyInt(L"x"));
		pBalance.setY(pResItem->getPropertyInt(L"y"));
		pBalance.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pBalance.setSkin(pResItem->getPropertyUInt(L"background"));		
		pBalance.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pBalance.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));
		pBalance.setZoneX(pResItem->getPropertyUInt(L"zonex"));
		pBalance.setZoneY(pResItem->getPropertyUInt(L"zoney"));
		pBalance.setZoneWidth(pResItem->getPropertyUInt(L"zonewidth"));
		pBalance.setZoneHeight(pResItem->getPropertyUInt(L"zoneheight"));
		pBalance.setSkinVerticalLine(pResItem->getPropertyUInt(L"vline"));
		pBalance.setSkinHorizontalLine(pResItem->getPropertyUInt(L"hline"));
		pBalance.setSkinSlider(pResItem->getPropertyUInt(L"slider"));

		initHitTestArea(pBalance, pResItem);
		initFilter(pBalance, pResItem);
	}

	void INIT::initScrollbar(CCtrlScrollbar &pScrollbar, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}		

		RETURN_IF_INITED_FROM_RESOURCE(&pScrollbar)

		pScrollbar.setID(uiID);
		pScrollbar.setVisible(pResItem->getPropertyBool(L"visible"));
		pScrollbar.setX(pResItem->getPropertyInt(L"x"));
		pScrollbar.setY(pResItem->getPropertyInt(L"y"));
		pScrollbar.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pScrollbar.setSkinUpArrow(pResItem->getPropertyUInt(L"uparrow"));
		pScrollbar.setSkinDownArrow(pResItem->getPropertyUInt(L"downarrow"));
		pScrollbar.setSkinBar(pResItem->getPropertyUInt(L"bar"));
		pScrollbar.setSkinSlider(pResItem->getPropertyUInt(L"slider"));
		pScrollbar.setScaleSlider(pResItem->getPropertyBool(L"scaleslider"));

		initHitTestArea(pScrollbar, pResItem);
		initFilter(pScrollbar, pResItem);
	}

	void INIT::initList(CCtrlList &pList, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pList)
		
		pList.setID(uiID);
		pList.setVisible(pResItem->getPropertyBool(L"visible"));
		pList.setX(pResItem->getPropertyInt(L"x"));
		pList.setY(pResItem->getPropertyInt(L"y"));
		pList.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pList.setSkin(pResItem->getPropertyUInt(L"background"));
		pList.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pList.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));

		initHitTestArea(pList, pResItem);
		initFilter(pList, pResItem);
	}

	void INIT::initList(CCtrlList_Simple &pList, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pList)

		pList.setID(uiID);
		pList.setVisible(pResItem->getPropertyBool(L"visible"));
		pList.setX(pResItem->getPropertyInt(L"x"));
		pList.setY(pResItem->getPropertyInt(L"y"));
		pList.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pList.setSkin(pResItem->getPropertyUInt(L"background"));
		pList.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pList.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));

		initHitTestArea(pList, pResItem);
		initFilter(pList, pResItem);
	}

	void INIT::initTable(CCtrlTable &pTable, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pTable)

		pTable.setID(uiID);
		pTable.setVisible(pResItem->getPropertyBool(L"visible"));
		pTable.setX(pResItem->getPropertyInt(L"x"));
		pTable.setY(pResItem->getPropertyInt(L"y"));
		pTable.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));
		pTable.setSkin(pResItem->getPropertyUInt(L"background"));
		pTable.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pTable.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));
		pTable.setCellWidth(pResItem->getPropertyUInt(L"cellwidth"));
		pTable.setCellHeight(pResItem->getPropertyUInt(L"cellheight"));

		initHitTestArea(pTable, pResItem);
		initFilter(pTable, pResItem);
	}

	void INIT::initTree(CCtrlTree &pTree, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pTree)

		pTree.setID(uiID);
		pTree.setVisible(pResItem->getPropertyBool(L"visible"));
		pTree.setX(pResItem->getPropertyInt(L"x"));
		pTree.setY(pResItem->getPropertyInt(L"y"));
		pTree.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pTree.setSkin(pResItem->getPropertyUInt(L"background"));
		pTree.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pTree.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));
		pTree.setLeftSpace(pResItem->getPropertyUInt(L"leftspace"));

		initHitTestArea(pTree, pResItem);
		initFilter(pTree, pResItem);
	}

	void INIT::initProgressbar(CCtrlProgressbar &pProgressbar, UINT uiID, CContainer *pParent)
	{
		CResItem *pResItem = findResItem(pParent, uiID);
		if (pResItem == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pProgressbar)

		pProgressbar.setID(uiID);
		pProgressbar.setVisible(pResItem->getPropertyBool(L"visible"));
		pProgressbar.setX(pResItem->getPropertyInt(L"x"));
		pProgressbar.setY(pResItem->getPropertyInt(L"y"));
		pProgressbar.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pProgressbar.setSkin(pResItem->getPropertyUInt(L"background"));
		pProgressbar.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pProgressbar.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));		
		pProgressbar.setBarArea(pResItem->getPropertyUInt(L"barx"), 
								pResItem->getPropertyUInt(L"bary"),
								pResItem->getPropertyUInt(L"barwidth"),
								pResItem->getPropertyUInt(L"barheight"));
		pProgressbar.setIsHorizontalPGB(!pResItem->getPropertyBool(L"verticalPGB"));
		pProgressbar.setSkinRegularBar(pResItem->getPropertyUInt(L"regularbar"));
		pProgressbar.setSkinRegularSlider(pResItem->getPropertyUInt(L"regularslider"));
		pProgressbar.setSkinIrregularBar(pResItem->getPropertyUInt(L"irregularbar"));
		pProgressbar.setSkinIrregularSlider(pResItem->getPropertyUInt(L"irregularslider"));

		initHitTestArea(pProgressbar, pResItem);
		initFilter(pProgressbar, pResItem);
	}

	void INIT::initButton(CCtrlButton &pButton, CResItem *pResItem)
	{
		if (pResItem == NULL)
		{
			return;
		}	

		RETURN_IF_INITED_FROM_RESOURCE(&pButton)

		pButton.setID(pResItem->getID());
		pButton.setName(pResItem->getName());
		pButton.setVisible(pResItem->getPropertyBool(L"visible"));
		pButton.setX(pResItem->getPropertyInt(L"x"));
		pButton.setY(pResItem->getPropertyInt(L"y"));
		pButton.setPreferredSize(pResItem->getPropertyUInt(L"width"), pResItem->getPropertyUInt(L"height"));		
		pButton.setSkin(pResItem->getPropertyUInt(L"background"));
		pButton.setFocusSkin(pResItem->getPropertyUInt(L"ccp"));
		pButton.setCCPBelowComponent(isCCPBelowComponent(pResItem->getPropertyString(L"ccpzorder")));

		initHitTestArea(pButton, pResItem);
		initFilter(pButton, pResItem);

		UINT ui = 0;
		for( ui=0; ui < pResItem->getTextFieldSize(); ui++ )
		{
			CResTextField *pResTextField = pResItem->getTextFieldByIndex(ui);

			initTextField(*(pButton.extractTextField(pResTextField->getName())), pResTextField);
		}

		for( ui=0; ui < pResItem->getImageFieldSize(); ui++ )
		{
			CResImageField *pResImageField = pResItem->getImageFieldByIndex(ui);

			initImageField(*(pButton.extractImageField(pResImageField->getName())), pResImageField);
		}
	}

	void INIT::initTextField(CCtrlTextField &pTextField, CResNode *pResNode)
	{
		if (pResNode == NULL)
		{
			return;
		}

		//RETURN_IF_INITED_FROM_RESOURCE(&pTextField)
		if (pTextField.getHasInitFromResource())
		{
			return;
		}

		pTextField.setID(pResNode->getID());
		pTextField.setVisible(pResNode->getPropertyBool(L"visible"));		
		pTextField.setX(pResNode->getPropertyInt(L"x"));
		pTextField.setY(pResNode->getPropertyInt(L"y"));
		pTextField.setPreferredSize(pResNode->getPropertyUInt(L"width"), pResNode->getPropertyUInt(L"height"));
		pTextField.setContent(pResNode->getPropertyString(L"content"));
		pTextField.setLineSpacing(pResNode->getPropertyInt(L"linespacing"));
		pTextField.setMultiLine(pResNode->getPropertyBool(L"multiline"));

		String strHLayout = pResNode->getPropertyString(L"hlayout");
		if (strHLayout == L"left")
		{
			pTextField.setHorizontalLayout(CRenderText::HL_LEFT);
		}
		else if (strHLayout == L"center")
		{
			pTextField.setHorizontalLayout(CRenderText::HL_CENTER);
		}
		else if (strHLayout == L"right")
		{
			pTextField.setHorizontalLayout(CRenderText::HL_RIGHT);
		}

		String strVLayout = pResNode->getPropertyString(L"vlayout");
		if (strVLayout == L"top")
		{
			pTextField.setVerticalLayout(CRenderText::VL_TOP);
		}
		else if (strVLayout == L"middle")
		{
			pTextField.setVerticalLayout(CRenderText::VL_MIDDLE);
		}
		else if (strVLayout == L"bottom")
		{
			pTextField.setVerticalLayout(CRenderText::VL_BOTTOM);
		}

		String strWrapLine = pResNode->getPropertyString(L"wrapline");
		if (strWrapLine == L"no-wrap")
		{
			pTextField.setWrapLine(CRenderText::WL_NOWRAP);
		}
		else if (strWrapLine == L"wrap-by-word")
		{
			pTextField.setWrapLine(CRenderText::WL_WRAP_BY_WORD);
		}
		else if (strWrapLine == L"wrap-by-char")
		{
			pTextField.setWrapLine(CRenderText::WL_WRAP_BY_CHAR);
		}

		String strBiDiMode = pResNode->getPropertyString(L"bidimode");
		if (strBiDiMode == L"left-to-right")
		{
			pTextField.setBiDiMode(CRenderText::BDM_LEFT_TO_RIGHT);
		}
		else if (strBiDiMode == L"right-to-left")
		{
			pTextField.setBiDiMode(CRenderText::BDM_RIGHT_TO_LEFT);
		}

		CResFontGroup *pResFontGroup = CSkinParser::getInstance()->getFontGroupByID(pResNode->getPropertyUInt(L"font"));
		if (pResFontGroup == NULL)
		{
			pTextField.setHasInitFromResource(TRUE);
			pTextField.setActiveFont(0);
			return;
		}

		for( UINT ui = 0; ui < pResFontGroup->getFontCount(); ui++ )
		{
			CResFont* pResFont = pResFontGroup->getFontByIndex(ui);

			String strStyle = pResFont->getFontStyle();
			CRenderFont::EFontStyle eFontStyle = CRenderFont::FS_REGULAR;
            if (strStyle == L"regular")
            {
				eFontStyle = CRenderFont::FS_REGULAR;
            }
            else if (strStyle == L"bold")
            {
				eFontStyle = CRenderFont::FS_BOLD;
            }
            else if (strStyle == L"italic")
            {
				eFontStyle = CRenderFont::FS_ITALIC;
            }
            else if (strStyle == L"bold-italic")
            {
				eFontStyle = CRenderFont::FS_BOLDITALIC;
            }

			pTextField.extractFontInfo(pResFont->getFontName(), pResFont->getFontColor(), pResFont->getFontSize(), eFontStyle);
		}

		pTextField.setHasInitFromResource(TRUE);

		pTextField.setActiveFont(0);		
	}

	void INIT::initImageField(CCtrlImageField &pImageField, CResNode *pResNode)
	{
		if (pResNode == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pImageField)

		pImageField.setID(pResNode->getID());
		pImageField.setVisible(pResNode->getPropertyBool(L"visible"));
		pImageField.setX(pResNode->getPropertyInt(L"x"));
		pImageField.setY(pResNode->getPropertyInt(L"y"));
		pImageField.setPreferredSize(pResNode->getPropertyUInt(L"width"), pResNode->getPropertyUInt(L"height"));
		pImageField.setSkin(pResNode->getPropertyUInt(L"img"));

		String strLayout = pResNode->getPropertyString(L"layout");
		CPropertySet *pResPropertySet = pResNode->getPropertySet(L"layout_param");
		if (strLayout == L"normal")
		{
			pImageField.setLayout(CRenderImage::L_NORMAL);

			if (pResPropertySet != NULL)
			{
				pImageField.setNormalLayoutClipOffsetX(pResPropertySet->getPropertyInt(L"clipx"));
				pImageField.setNormalLayoutClipOffsetY(pResPropertySet->getPropertyInt(L"clipy"));
			}
		}
		else if (strLayout == L"stretch-center")
		{
			pImageField.setLayout(CRenderImage::L_STRETCH_CENTER);
		}
		else if (strLayout == L"stretch-fill")
		{
			pImageField.setLayout(CRenderImage::L_STRETCH_FILL);
		}	
	}

	void INIT::initWebBrowser(CCtrlWebBrowser &pWebBrowser, CResNode *pResNode)
	{
		if (pResNode == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pWebBrowser)

		pWebBrowser.setID(pResNode->getID());
		pWebBrowser.setVisible(pResNode->getPropertyBool(L"visible"));
		pWebBrowser.setX(pResNode->getPropertyInt(L"x"));
		pWebBrowser.setY(pResNode->getPropertyInt(L"y"));
		pWebBrowser.setPreferredSize(pResNode->getPropertyUInt(L"width"), pResNode->getPropertyUInt(L"height"));
		pWebBrowser.setWebBrsID(pResNode->getPropertyInt(L"webBrsID"));
	}

	void INIT::initEdit(CCtrlEdit &pEdit, CResNode *pResNode)
	{
		if (pResNode == NULL)
		{
			return;
		}

		RETURN_IF_INITED_FROM_RESOURCE(&pEdit)

		pEdit.setID(pResNode->getID());
		pEdit.setVisible(pResNode->getPropertyBool(L"visible"));
		pEdit.setX(pResNode->getPropertyInt(L"x"));
		pEdit.setY(pResNode->getPropertyInt(L"y"));
		pEdit.setPreferredSize(pResNode->getPropertyUInt(L"width"), pResNode->getPropertyUInt(L"height"));
		pEdit.setEditID(pResNode->getPropertyInt(L"editID"));
	}

	void INIT::initHitTestArea(CComponent &pControl, CResItem *pResItem)
	{
		if (pResItem == NULL)
		{
			return;
		}

		CResHitArea *pResHitArea = NULL;
		for( UINT uiIndex = 0; uiIndex < pResItem->getHitAreaSize(); uiIndex++ )
		{
			pResHitArea = pResItem->getHitAreaByIndex(uiIndex);

			String strType = pResHitArea->getPropertyString(L"type");

			if( strType == L"RECTANGLE")
			{
				CHitAreaRectangle* pHTARect = new CHitAreaRectangle();
				pHTARect->setX(pResHitArea->getPropertyInt(L"x"));
				pHTARect->setY(pResHitArea->getPropertyInt(L"y"));
				pHTARect->setHeight(pResHitArea->getPropertyUInt(L"height"));
				pHTARect->setWidth(pResHitArea->getPropertyUInt(L"width"));
				pControl.addHitTestArea(pHTARect);
			}
			else if( strType == L"CIRCLE")
			{
				CHitAreaCircle* pHTACircle = new CHitAreaCircle();
				pHTACircle->setCenterPoint(pResHitArea->getPropertyInt(L"x"), pResHitArea->getPropertyInt(L"y"));
				pHTACircle->setRadius(pResHitArea->getPropertyUInt(L"radius"));
				pControl.addHitTestArea(pHTACircle);
			}
			else if( strType == L"TRIANGLE")
			{
				CHitAreaTriangle* pHTATriangle = new CHitAreaTriangle();
				pHTATriangle->setVertex1(pResHitArea->getPropertyInt(L"x1"), pResHitArea->getPropertyInt(L"y1"));
				pHTATriangle->setVertex2(pResHitArea->getPropertyInt(L"x2"), pResHitArea->getPropertyInt(L"y2"));
				pHTATriangle->setVertex3(pResHitArea->getPropertyInt(L"x3"), pResHitArea->getPropertyInt(L"y3"));
				pControl.addHitTestArea(pHTATriangle);
			}
		}
	}

	void INIT::initFilter(CComponent &pControl, CResItem *pResItem)
	{
		if (pResItem == NULL)
		{
			return;
		}

		for( UINT uiIndex = 0; uiIndex < pResItem->getFilterSize(); uiIndex++ )
		{
			initFilter(pControl, pResItem->getFilterByIndex(uiIndex));
		}
	}

	void INIT::initFilter(CComponent &pControl, CResGroup *pResGroup)
	{
		if (pResGroup == NULL)
		{
			return;
		}

		for( UINT uiIndex = 0; uiIndex < pResGroup->getFilterSize(); uiIndex++ )
		{
			initFilter(pControl, pResGroup->getFilterByIndex(uiIndex));
		}
	}

	void INIT::initFilter(CComponent &pControl, CResFilter *pResFilter)
	{
		if (pResFilter == NULL)
		{
			return;
		}

		String strType = pResFilter->getPropertyString(L"type");

		if( strType == L"REFLECTION")
		{
			CRenderFltReflection* pFilter = new CRenderFltReflection();

			pFilter->setHeight(pResFilter->getPropertyUInt(L"height"));

			ObjectPtr<CMovieClip> pMovieClip(ENV::getMovieClip(pResFilter->getPropertyUInt(L"maskimg")));
			if (pMovieClip != NULL)
			{
				pFilter->setMaskImage(pMovieClip->getCurrentImage());
			}

			pControl.addFilter(pFilter);
		}
		else if( strType == L"GLASS")
		{
			CRenderFltGlass* pFilter = new CRenderFltGlass();
			
			pFilter->setBlurSize(pResFilter->getPropertyUInt(L"blursize"));
		 
			ObjectPtr<CMovieClip> pMovieClip( ENV::getMovieClip(pResFilter->getPropertyUInt(L"maskimg")) );
			if (pMovieClip != NULL)
			{
				pFilter->setMaskImage(pMovieClip->getCurrentImage());
			}

			pControl.addFilter(pFilter);
		}
		else if( strType == L"RIPPLE")
		{
			CRenderFltRipple* pFilter = new CRenderFltRipple();

			pFilter->setTouchRadius(pResFilter->getPropertyUInt(L"touchradius"));
			pFilter->setDamping(pResFilter->getPropertyUInt(L"damping"));

			pControl.addFilter(pFilter);
		}
		else if( strType == L"MASK")
		{
			CRenderFltMask* pFilter = new CRenderFltMask();

			ObjectPtr<CMovieClip> pMovieClip(ENV::getMovieClip(pResFilter->getPropertyUInt(L"maskimg")));
			if (pMovieClip != NULL)
			{
				pFilter->setMaskImage(pMovieClip->getCurrentImage());
			}

			pControl.addFilter(pFilter);
		}
	}

	CResItem* INIT::findResItem(CContainer *pParent, UINT uiItemID)
	{
		if (pParent == NULL)
		{
			return NULL;
		}

		CResGroup *pResGroup = NULL;

		switch (pParent->getType())
		{
		case CONFIG::CONTAINER_TYPE_SCREEN:
			pResGroup = CSkinParser::getInstance()->getScreenByID(pParent->getID());
			break;

		case CONFIG::CONTAINER_TYPE_POPUP:
			pResGroup = CSkinParser::getInstance()->getPopupByID(pParent->getID());
			break;

		case CONFIG::CONTAINER_TYPE_FUNCWIDGET:
			pResGroup = CSkinParser::getInstance()->getFuncWidgetByID(pParent->getID());
			break;

		case CONFIG::CONTAINER_TYPE_CTRLWIDGET:
			pResGroup = CSkinParser::getInstance()->getCtrlWidgetByID(pParent->getID());
			break;

		default:
			return NULL;
		}

		if (pResGroup == NULL)
		{
			return NULL;
		}

		return pResGroup->getItemByID(uiItemID);
	}

	BOOLEAN INIT::isCCPBelowComponent(const String & strValue)
	{
		if (strValue == L"over")
		{
			return FALSE;
		}
		else if (strValue == L"below")
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}