/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API INIT
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static	void	initMainFrame(CBaseMainFrame *pMainFrame, UINT uiID);
		static	void	initScreen(CBaseScreen *pScreen, UINT uiID);
		static	void	initPopup(CBasePopup *pPopup, UINT uiID);
		static void     initMessageBox(CWgtMessageBox *pMessageBox, UINT uiID);		
		static	void	initFuncWidget(CBaseFuncWidget *pFuncWidget, UINT uiID);		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static	void	initFuncWidget(CBaseFuncWidget &pFuncWidget, UINT uiID, CContainer *pParent);
		static void     initKeyboard(CWgtKeyboard &pKeyboard, UINT uiID, CContainer* pParent);
		static	void    initSelector(CCtrlSelector &pSelector, UINT uiID, CContainer *pParent);
		static	void	initButton(CCtrlButton &pButton, UINT uiID, CContainer *pParent);
		static	void	initLabel(CCtrlLabel &pLabel, UINT uiID, CContainer *pParent);
		static	void	initTextField(CCtrlTextField &pTextField, UINT uiID, CContainer *pParent);
		static	void	initImageField(CCtrlImageField &pImageField, UINT uiID, CContainer *pParent);
		static	void	initViewport(CCtrlViewport &pViewport, UINT uiID, CContainer *pParent);
		static	void	initBalance(CCtrlBalance &pBalance, UINT uiID, CContainer *pParent);
		static	void    initScrollbar(CCtrlScrollbar &pScrollbar, UINT uiID, CContainer *pParent);
		static	void	initList(CCtrlList &pList, UINT uiID, CContainer *pParent);
		static	void	initList(CCtrlList_Simple &pList, UINT uiID, CContainer *pParent);
		static	void	initTable(CCtrlTable &pTable, UINT uiID, CContainer *pParent);
		static	void	initTree(CCtrlTree &pTree, UINT uiID, CContainer *pParent);
		static	void	initProgressbar(CCtrlProgressbar &pProgressbar, UINT uiID, CContainer *pParent);
		static  void	initWebBrowser(CCtrlWebBrowser &pWebBrowser, UINT uiID, CContainer *pParent);
		static  void    initEdit(CCtrlEdit &pEdit, UINT uiID, CContainer *pParent);
		
	protected:// method
		INIT(){}
		INIT(const INIT&){}
		virtual ~INIT(){}		
		INIT& operator =(const INIT&){return *this;}

	private:// method
		static	void			initButton(CCtrlButton &pButton, CResItem *pResItem);
		static	void			initTextField(CCtrlTextField &pTextField, CResNode *pResNode);
		static	void			initImageField(CCtrlImageField &pImageField, CResNode *pResNode);
		static	void			initWebBrowser(CCtrlWebBrowser &pWebBrowser, CResNode *pResNode);
		static  void			initEdit(CCtrlEdit &pEdit, CResNode *pResNode);
		static	void			initHitTestArea(CComponent &pControl, CResItem *pResItem);
		static	void			initFilter(CComponent &pControl, CResItem *pResItem);
		static	void			initFilter(CComponent &pControl, CResGroup *pResGroup);
		static	void			initFilter(CComponent &pControl, CResFilter *pResFilter);

		static	CResItem*		findResItem(CContainer *pParent, UINT uiItemID);

		static	BOOLEAN			isCCPBelowComponent(const String & strValue);		

	protected:// property

	private:// property	

	};
}