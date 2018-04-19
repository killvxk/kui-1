#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"
#include "IdcMenuXml.h"

namespace OpenHMI
{
	class CFuncWgtAdRec
		: public CBaseFuncWidget
		, IViewTable
		, OpenHMI::ITimerListener
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here
		struct gifItem
		{
			std::vector<GifImage>			animate_images;
			int								animate_index;
		};

		struct adItem
		{
			adRc item;
			BOOLEAN bGif;
			gifItem gif;
		};


	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_BTN_ITEM							 = 1 ,
			RES_ITM_TIPS								 = 2 ,
			RES_ITM_AD									 = 3 ,
			RES_ITM_BTN_CHANGE							 = 4 ,

		};

	public:// method
		CFuncWgtAdRec();
		virtual ~CFuncWgtAdRec();

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

		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);

		// TODO: define your public method here
		void changeSize(UINT w, UINT h);

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void onBtnTableAdItemTchRel(CComponent *pTarget);
		void onBtnChangeHoverFocused(CComponent *pTarget);
		void onBtnChangeHoverCanceled(CComponent *pTarget);
		void onBtnChangeTouchRel(CComponent *pTarget);
		// TODO: define logic event listener here

		// TODO: define your normal private method here
		void					initAdItem();
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgTips;
		CCtrlTable							m_tableAd;
		CCtrlButton							m_btnChange;


		// TODO: define normal private property here
		Array<CTimer>						m_aryGifTimer;

		Array<adItem>						m_aryAdItem;

	};
}
