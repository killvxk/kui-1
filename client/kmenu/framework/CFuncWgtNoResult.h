#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtNoResult
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
			RES_ITM_TIPS								 = 1 ,
			RES_ITM_TABLE_GAME							 = 3 ,
			RES_ITM_BTN_ITEM							 = 4 ,

		};

	public:// method
		CFuncWgtNoResult();
		virtual ~CFuncWgtNoResult();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();
		virtual	ICursorTracker*	probe(INT iX, INT iY);

		// Declare functions for interface IViewTable
		virtual CComponent* getItemComponent(CCtrlTable* pTable, Object* pObjItem);

		virtual void drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable);


		// TODO: define your public method here
		void changeSize(INT w, INT h);
	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void onBtnTableItemTchRel(CComponent *pTarget);

		// TODO: define logic event listener here

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgTips;
		CCtrlTable							m_tableGame;


		// TODO: define normal private property here
		std::vector<tagGameInfo*>			m_games;
	};
}
