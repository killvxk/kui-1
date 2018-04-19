#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtChangeSkin
		: public CBaseFuncWidget
		, IViewTable
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_IMG_LOG								 = 1 ,
			RES_ITM_TXT_TITLE							 = 2 ,
			RES_ITM_SCROLLBAR							 = 5 ,
			RES_ITM_TABLE								 = 7 ,
			RES_ITM_BTN_QUIT							 = 9 ,
			RES_ITM_SKIN_ITEM							 = 14 ,
			RES_ITM_BTN_SAVE							 = 15 ,

		};

	public:// method
		CFuncWgtChangeSkin();
		virtual ~CFuncWgtChangeSkin();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// Declare functions for interface IViewTable
		virtual CComponent* getItemComponent(CCtrlTable* pTable, Object* pObjItem);

		virtual void drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable);


		// TODO: define your public method here
		void quit();

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnSaveTchRel(CComponent *pTarget);
		void	onAdminDsy(CBasePopup *pTarget);

		// TODO: define logic event listener here
		void	onSkinChange();
		void	onSkinAdd();
		void	onSkinDel();
		void	onUpdateData();
		void	onLoginPopAdminOK();
		void	onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgLog;
		CCtrlTextField						m_txtTitle;
		CCtrlScrollbar						m_scrollbar;
		CCtrlTable							m_table;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnSave;


		// TODO: define normal private property here
		CPopAdmin*							m_pPopAdmin;
	};
}
