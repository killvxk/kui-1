#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtReadFile
		: public CBaseFuncWidget
		, IViewList_Simple
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_BTN_QUIT							 = 1 ,
			RES_ITM_BTN_READ							 = 2 ,
			RES_ITM_TITLE								 = 3 ,
			RES_ITM_BTN_CANCEL							 = 4 ,
			RES_ITM_FILE								 = 5 ,
			RES_ITM_BTN_ITEM							 = 6 ,
			RES_ITM_SCROLLBAR_LIST						 = 7 ,

		};

	public:// method
		CFuncWgtReadFile();
		virtual ~CFuncWgtReadFile();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// Declare functions for interface IViewList_Simple
		virtual CComponent* getItemComponent(CCtrlList_Simple* pList);

		virtual void drawItem(CCtrlList_Simple* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus);


		// TODO: define your public method here
		void readFile(DWORD gid) { m_Gid = gid; }
	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnReadTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);
		void	onBtnListFileItemTchRel(CComponent *pTarget);
		void	onPopTipsDsy(CBasePopup *pTarget);

		// TODO: define logic event listener here
		void	onKeyEnter();
		void	onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnRead;
		CCtrlTextField						m_txtTitle;
		CCtrlButton							m_btnCancel;
		CCtrlList_Simple					m_listFile;
		CCtrlScrollbar						m_scrollbarList;


		// TODO: define normal private property here
		DWORD								m_Gid;
		Array<Object>						m_aryData;
		Array2<CString>						m_aryFileInfo;
		CPopTips*							m_pPopTips;
	};
}
