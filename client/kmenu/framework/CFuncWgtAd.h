#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"
#include "IdcMenuXml.h"

namespace OpenHMI
{
	class CFuncWgtAd
		: public CBaseFuncWidget
		, IViewTable
		, OpenHMI::ITimerListener
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_IMG_BG								 = 6 ,
			RES_ITM_AD									 = 3 ,
			RES_ITM_BTN_ITEM							 = 7 ,
		};

		struct gifItem
		{
			std::vector<GifImage>			animate_images;
			int								animate_index;
		};

		struct adItem
		{
			ads item;
			BOOLEAN bGif;
			gifItem gif;
		};

	public:// method
		CFuncWgtAd();
		virtual ~CFuncWgtAd();

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
		// TODO: define logic event listener here
		void	onSetFocus();
		void	onKillFocus();

		// TODO: define your normal private method here
		void					initAdItem();
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CCtrlTable							m_tableAd;


		// TODO: define normal private property here
		Array<CTimer>						m_aryGifTimer;

		Array<adItem>						m_aryAdItem;
	};
}
