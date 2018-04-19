/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventBalance
	{
	public:
		static const UINT POS_CHANGED				= 0;
	};

	class OPENHMI_API CCtrlBalance : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlBalance();
		virtual ~CCtrlBalance();
		
		CModelBalance*	getModel() { return &m_model; };
	
		/**
		*  Inner method.
		*/
		void	setSkinVerticalLine(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void	setSkinHorizontalLine(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void	setSkinSlider(UINT uiImageID);

		/**
		*  Inner method.
		*/
		void	setZoneX(UINT uiValue);

		/**
		*  Inner method.
		*/
		void	setZoneY(UINT uiValue);

		/**
		*  Inner method.
		*/
		void	setZoneWidth(UINT uiValue);

		/**
		*  Inner method.
		*/
		void	setZoneHeight(UINT uiValue);		

		/**
		* Whether circle adjust while adjusting CCP.
		* Circle adjust means the maximum and minimum of the range is jointed to be a circle, it circles when adjust CCP.
		*/
		void	circleAdjust(BOOLEAN bValue);

		/**
		* Set the highlight status of the vertical line.
		*/
		void    setVLineHighLight(BOOLEAN bValue);

		/**
		* Set the highlight status of the horizontal line.
		*/
		void    setHLineHighLight(BOOLEAN bValue);
		
		// virtual function
		virtual	void	refresh();
		virtual	void	littleRefresh();
		virtual	void	destroySkin();
		virtual	void	destroy();
		virtual void	buildRenderTree(CRenderGroup &renderGroup);

		virtual	void	setFocus(ECCPOperation eCCPOperation = CCP_ROTATE_RIGHT);
		virtual void	releaseFocus();

		virtual BOOLEAN turnFocusToLeft(UINT uiStep);
		virtual BOOLEAN turnFocusToRight(UINT uiStep);

		virtual void	doFocusEnterPress();

		virtual	void	onTouchPress(INT iX, INT iY);
		virtual	void	onTouchPressRelease(INT iX, INT iY);
		virtual	void	onTouchLongPressRelease(INT iX, INT iY);
		virtual void    onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual	void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);

	protected:// method
		CCtrlBalance(const CCtrlBalance&){}
		CCtrlBalance& operator =(const CCtrlBalance&){return *this;}

	private:// method
		void	resetPos(INT iX, INT iY);
		void    compose();
		void    onTouchRelease();
		void    updateView();
		void	ccpHighLightLine();

	protected:// property

	private:// property
		CModelBalance			m_model;
		CViewBalance			m_view;

		ObjectPtr<CMovieClip>	m_pMCVerticalLine;
		ObjectPtr<CMovieClip>	m_pMCHorizontalLine;
		ObjectPtr<CMovieClip>	m_pMCSlider;

		CRenderImage			m_renderImageVLine;
		CRenderImage			m_renderImageHLine;
		CRenderImage			m_renderImageSlider;

		INT						m_iZoneX;
		INT						m_iZoneY;
		INT						m_iZoneWidth;
		INT						m_iZoneHeight;

		BOOLEAN					m_bDragSlider;
		BOOLEAN					m_bCCCHoritontal;
		BOOLEAN					m_bCircleAdjust;
		BOOLEAN					m_bVLineHighLight;
		BOOLEAN					m_bHLineHighLight;		
	};
}