#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtSkinItem
		: public CBaseFuncWidget
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_BTN_ITEM							 = 1 ,
			RES_ITM_BTN_DEL								 = 2 ,
			RES_ITM_BTN_ADD								 = 4 ,

		};

	public:// method
		CFuncWgtSkinItem();
		virtual ~CFuncWgtSkinItem();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();
		virtual	void	onLostHover();
		virtual	void	onMove(INT iX, INT iY);

		// TODO: define your public method here
		CCtrlButton* getBtnItem() { return &m_btnItem; }
		CCtrlButton* getBtnAdd() { return &m_btnAdd; }
		void hideDelBtn() { m_btnDel.setVisible(FALSE); }
		void showAddBtn(BOOLEAN bShow) { m_btnAdd.setVisible(bShow); }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void onBtnItemTchPre(CComponent *pTarget);
		void onBtnDelTchPre(CComponent *pTarget);
		void onBtnAddTchRel(CComponent *pTarget);

		// TODO: define logic event listener here

		// TODO: define your normal private method here
		static unsigned int __stdcall threadProc(void *pParam);

	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlButton							m_btnItem;
		CCtrlButton							m_btnDel;
		CCtrlButton							m_btnAdd;


		// TODO: define normal private property here
	};
}
