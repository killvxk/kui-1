#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtHotSearch
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
			RES_ITM_IMG_TAG								 = 10 ,
			RES_ITM_IMG_BG								 = 12 ,
			RES_ITM_MENU								 = 2 ,
			RES_ITM_BTN_ITEM							 = 4 ,
			RES_ITM_IMG_FOOT							 = 13 ,

		};

	public:// method
		CFuncWgtHotSearch();
		virtual ~CFuncWgtHotSearch();

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
		virtual	ICursorTracker*	probe(INT iX, INT iY);
		//virtual void hoverCheck(INT iX, INT iY);

		// TODO: define your public method here
		void enter() { m_listMenu.setCurSel(-1); m_listMenu.littleRefresh(); }
	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void onBtnListMenuItemTchRel(CComponent *pTarget);

		// TODO: define logic event listener here

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgTag;
		CCtrlImageField						m_imgBg;
		CCtrlList_Simple					m_listMenu;
		CCtrlImageField						m_imgFoot;


		// TODO: define normal private property here
		Array<Object>						m_aryData;
	};
}
