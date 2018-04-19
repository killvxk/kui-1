/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventScrollbar
	{
	public:
		static const UINT POS_CHANGED				= 0;
		static const UINT POS_CONTINUE_CHANGE_ENDED = 1;
	};

	class OPENHMI_API CCtrlScrollbar : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EPosChanged
		{
			PC_UNKNOW				   = 0,
			PC_LINE_UP                 = 1,
			PC_CONTINUE_LINE_UP		   = 2,
			PC_LINE_DOWN			   = 3,
			PC_CONTINUE_LINE_DOWN	   = 4,
			PC_PAGE_UP				   = 5,
			PC_PAGE_DOWN			   = 6,
			PC_SLIDER				   = 7,
		};

	private:// embed class or struct or enum

	public:// method
		CCtrlScrollbar();
		virtual ~CCtrlScrollbar();

		CModelScrollbar*	getModel() { return &m_model; }
		
		/**
		*  Inner method.
		*/
		void				setSkinUpArrow(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void			    setSkinDownArrow(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void			    setSkinBar(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void			    setSkinSlider(UINT uiImageID);

		/**
		* Whether need scale the slider to make the indicate effect more precise or not.
		*/
		void				setScaleSlider(BOOLEAN bValue);
		BOOLEAN				getScaleSlider() const { return m_bScaleSlider; }

		/**
		* Adjust mode means the scroll bar will response to user operation.
		* Non adjust mode means the scroll bar will not response to user operation.
		* Non adjust mode scroll control is usually used as the indicator for list item count and current list item shown without adjusting current list item through scroll control.
		*/
		BOOLEAN				isAdjustMode();
		void				setIsAdjustMode(BOOLEAN bValue);

		void				setCanDragSlider(BOOLEAN bValue) { m_bCanDragSlider = bValue; }

		void				movePageWhenClickUpDownArrow(BOOLEAN bValue) { m_bMoveByPage = bValue; }

		/**
		* @brief	Get the type of the cause for position change event.
		*
		* @return	PC_UNKNOW					Unknown cause.			
		*			PC_LINE_UP					Press the up arrow.
		*			PC_CONTINUE_LINE_UP			Long press the up arrow.
		*			PC_LINE_DOWN				Press the down arrow.
		*			PC_CONTINUE_LINE_DOWN		Long press the down arrow.
		*			PC_PAGE_UP					Press the area above the slider of the line.
		*			PC_PAGE_DOWN				Press the area below the slider of the line. 
		*			PC_SLIDER					Move the slider.
		*/
		EPosChanged			getPosChangedType() const { return m_ePosChangedType; }		

		/**
		* Used to move position by step distance to up.
		*/
		void				moveToPreviousStep();

		/**
		* Used to move position by step distance to down.
		*/
		void				moveToNextStep();

		/**
		* Used to move position by page distance to up.
		*/
		void				moveToPreviousPage();

		/**
		* Used to move position by page distance to down.
		*/
		void				moveToNextPage();		
		
		// virtual function
		virtual	void		refresh();
		virtual	void		littleRefresh();
		virtual	void		destroySkin();
		virtual	void		destroy();
		virtual void		buildRenderTree(CRenderGroup &renderGroup);

		virtual void		onMove(INT iX, INT iY);
		virtual	void		onTouchPress(INT iX, INT iY);
		virtual	void		onTouchPressRelease(INT iX, INT iY);
		virtual void	    onTouchLongPress(INT iX, INT iY);
		virtual void	    onTouchContinuePress(INT iX, INT iY);
		virtual	void		onTouchLongPressRelease(INT iX, INT iY);
		virtual void	    onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual	void		onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);

	protected:// method
		CCtrlScrollbar(const CCtrlScrollbar&){}
		CCtrlScrollbar& operator =(const CCtrlScrollbar&){return *this;}

	private:// method
		void		setPosChangedType(EPosChanged ePosChanged);
		void		compose();
		void		onTouchRelease();

	protected:// property

	private:// property
		CModelScrollbar			m_model;
		CViewScrollbar			m_view;
		
		ObjectPtr<CMovieClip>	m_pMCUpArrow;
		ObjectPtr<CMovieClip>	m_pMCDownArrow;
		ObjectPtr<CMovieClip>	m_pMCBar;
		ObjectPtr<CMovieClip>	m_pMCSlider;

		CRenderImage			m_renderImgUpArrow;
		CRenderImage			m_renderImgDownArrow;
		CRenderImage			m_renderImgBar;
		CRenderImage			m_renderImgSlider;

		EPosChanged				m_ePosChangedType;

		BOOLEAN					m_bCanDragSlider;
		INT						m_iFocusSelBeginPosY;
		INT						m_iFocusSelModelBeginPos;		
		BOOLEAN					m_bDragSlider;
		BOOLEAN					m_bScaleSlider;
		BOOLEAN					m_bLongPressHitUpArrow;
		BOOLEAN					m_bLongPressHitDownArrow;
		BOOLEAN					m_bMoveByPage;
		BOOLEAN					m_bUpArrowFocus;
		BOOLEAN					m_bDownArrowFocus;
	};
}