/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventProgressbar
	{
	public:
		static const UINT POS_CHANGED				= 0;
		static const UINT POS_ADJUSTED				= 1;
	};

	class OPENHMI_API CCtrlProgressbar : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlProgressbar();
		virtual ~CCtrlProgressbar();

		CModelProgressbar*	getModel()									{ return &m_model; }

		/**
		*  Inner method.
		*/
		void				setSkinRegularBar(UINT uiImageID);
		
		/**
		*  Inner method.
		*/
		void				setSkinRegularSlider(UINT uiImageID);
		
		/**
		*  Inner method.
		*/
		void				setSkinIrregularBar(UINT uiImageID);
		
		/**
		*  Inner method.
		*/
		void				setSkinIrregularSlider(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void				setBarArea(UINT uiX, UINT uiY, UINT uiWidth, UINT uiHeight);

		/**
		*  Inner method.
		*/
		void				setIsHorizontalPGB(BOOLEAN bValue)			{ m_bIsHorizontalPGB = bValue; }
		BOOLEAN				getIsHorizontalPGB() const					{ return m_bIsHorizontalPGB; }
		
		/**
		* @brief	Whether the progress bar is in adjust mode or status mode.
		*
		* @return	TRUE	Means adjust mode, in this mode, when progress bar has CCP focus and CCP rotates, the progress bar will dispatch POS_ADJUST event.
		* 					The progress bar will tell the outer the value difference but not update its value until the value changed by outer program.
		* 					Here adjust mode only means the progress bar can be adjusted by CCP or screen touch / touch move.
		* 		   FALSE	Means status mode, in this mode, when progress bar has CCP focus and CCP rotates, the progress bar will not response to it. (neither screen touch).
		* 					The value shows in progress bar is always the current value.
		*/		
		void				setIsAdjustMode(BOOLEAN bValue);
		BOOLEAN				isAdjustMode() const						{ return m_bIsAdjustMode; }

		/**
		* @brief	Whether position alter while adjusting CCP.
		*
		* bValue:	TRUE	Means in adjust mode, the position in progress bar will change immediately while adjust by CCP rotates.
		* 			FALSE	Means in adjust mode, the position in progress bar will not change until the outer program changed the actual value.
		*/
		void				setIsPosAlterWhileAdjustCCP(BOOLEAN bValue)	{ m_bIsPosAlterWhileAdjustCCP = bValue; }
		BOOLEAN				isPosAlterWhileAdjustCCP() const			{ return m_bIsPosAlterWhileAdjustCCP; }

		/**
		* When adjust mode and position not alter while adjust CCP, you can use this value to get the step adjusted.
		*/
		INT					getAdjustStep() const						{ return m_iAdjustStep; }

		/**
		* Whether circle adjust while adjusting CCP.
		* Circle adjust means the maximum and minimum of the range is jointed to be a circle, it circles when adjust CCP.
		*/
		void				setIsCCPCircleAdjust(BOOLEAN bValue)		{ m_bCircleAdjust = bValue; }
		BOOLEAN				getIsCCPCircleAdjust() const				{ return m_bCircleAdjust; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void				setProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep = 0);
		void				setPos(INT iValue);
		INT					getPos()									{ return m_model.getPos(); }
		INT					getMin()									{ return m_model.getMin(); }
		INT					getMax()									{ return m_model.getMax(); }		

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	void	refresh();
		virtual	void	littleRefresh();
		virtual	void	destroySkin();
		virtual	void	destroy();
		virtual void	buildRenderTree(CRenderGroup &renderGroup);

		virtual BOOLEAN	turnFocusToLeft(UINT uiStep);
		virtual BOOLEAN	turnFocusToRight(UINT uiStep);

		virtual	void	onTouchPress(INT iX, INT iY);
		virtual	void	onTouchPressRelease(INT iX, INT iY);
		virtual	void	onTouchLongPressRelease(INT iX, INT iY);
		virtual	void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual	void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);

	protected:// method
		CCtrlProgressbar(const CCtrlProgressbar&){}
		CCtrlProgressbar& operator =(const CCtrlProgressbar&){return *this;}

	private:// method
		void			compose();
		void			onTouchRelease();

	protected:// property

	private:// property
		CModelProgressbar		m_model;
		CViewProgressbar		m_view;
		
		ObjectPtr<CMovieClip>	m_pMCRegularBar;
		ObjectPtr<CMovieClip>	m_pMCRegularSlider;
		ObjectPtr<CMovieClip>	m_pMCIrregularBar;
		ObjectPtr<CMovieClip>	m_pMCIrregularSlider;
		
		CRenderImage			m_renderImageRegularBar;
		CRenderImage			m_renderImageRegularSlider;
		CRenderImage			m_renderImageIrregularBar;
		CRenderImage			m_renderImageIrregularSlider;

		CRectangle				m_rcBarArea;
		INT						m_iClipY;

		BOOLEAN					m_bCircleAdjust;
		BOOLEAN					m_bIsAdjustMode;
		BOOLEAN					m_bIsPosAlterWhileAdjustCCP;
		INT						m_iAdjustStep;

		BOOLEAN					m_bDragSlider;
		BOOLEAN					m_bIsHorizontalPGB;

		INT						m_iPosRecordNew;
		BOOLEAN					m_bUpdateLockOn;		
	};
}