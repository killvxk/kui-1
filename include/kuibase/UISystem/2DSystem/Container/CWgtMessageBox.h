/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CWgtMessageBox 
					: public CContainer
					, public ITimerListener
	{
	public:// const define
		static const INT		AUTO_DISAPPEAR_SECOND			= 3;

	private:// const define

	public:// embed class or struct or enum
		enum EAnimationStyle
		{
			AS_ALFA				= 0x01,
			AS_ROTATE			= 0x02,
			AS_SCALE			= 0x04,
		};

		struct tagMessageBoxParam
		{
			String			strTitle;
			String			strContent;
			INT				iAutoDisappearSecond;
			String			strOk;
			PCALLBACK		onOk;
			Object*			pOkParam;
			INT				iBeepOk;
			String			strCancel;
			PCALLBACK		onCancel;
			Object*			pCancelParam;
			INT				iBeepCancel;
			String			strAlternate;
			PCALLBACK		onAlternate;
			Object*			pAlternateParam;
			INT				iBeepAlternate;
			UINT			uiAnimationStyle;
			UINT			uiIntervalTime;
			UINT			uiRepeatCount;
			INT				iStartX;
			INT				iStartY;
			UINT			uiStartOpacity;
			FLOAT			fStartScale;
			UINT			uiCircleCnt;
			BOOLEAN			bNeedDestroyAnimation;

			tagMessageBoxParam() : strTitle(STRING_NULL)
								 , strContent(STRING_NULL)
								 , iAutoDisappearSecond(0)
								 , strOk(STRING_NULL)
								 , onOk(NULL)
								 , pOkParam(NULL)
								 , iBeepOk(0)
								 , strCancel(STRING_NULL)
								 , onCancel(NULL)
								 , pCancelParam(NULL)
								 , iBeepCancel(0)
								 , strAlternate(STRING_NULL)
								 , onAlternate(NULL)
								 , pAlternateParam(NULL)
								 , iBeepAlternate(0)
								 , uiAnimationStyle(0)
								 , uiIntervalTime(10)
								 , uiRepeatCount(20)
								 , iStartX(-1)
								 , iStartY(-1)
								 , uiStartOpacity(100)
								 , fStartScale(1.0f)
								 , uiCircleCnt(0)
								 , bNeedDestroyAnimation(FALSE)
			{
				;
			}
		};
	private:// embed class or struct or enum

	public:// method
		CWgtMessageBox();
		virtual ~CWgtMessageBox();

		DECLARE_EVENT_MAP

		/**
		* @brief	Set the message box parameter.
		*
		* @param	strTitle				The title of this message box.
		* @param	strContent				The content of this message box.
		* @param	strOk					The string for OK button.
		* @param	strCancel				The string for cancel button.
		* @param	strAlternate			The string for alternate button.
		* @param	ibtnOkSoundType			The sound type for OK button when clicked.
		* @param	ibtnCancelSoundType		The sound type for cancel button when clicked.
		* @param	ibtnAlternateSoundType	The sound type for alternate button when clicked.
		*/
		void	setParam(	const String&	strTitle				= STRING_NULL, 
							const String&	strContent				= STRING_NULL, 
							const String&	strOk					= STRING_NULL, 
							const String&	strCancel				= STRING_NULL, 
							const String&	strAlternate			= STRING_NULL,
							INT				ibtnOkSoundType			= -1,
							INT				ibtnCancelSoundType		= -1,
							INT				ibtnAlternateSoundType	= -1);

		/**
		* @brief	Set the animation info while message box show.
		*
		* @param	uiAnimationStyle	A combination of EAnimationStyle, you can select the style you want and make them a combination use "or" operation.
		* @param	uiIntervalTime		The interval time among each animation frame, unit is millisecond.
		* @param	uiRepeatCount		The animation frame count.
		* @param	iStartX				X of the start point coordinates. the start point means when the animation starts, the point where the message box first appear. This value will always work no matter what animation style selected.
		* @param	iStartY				Y of the start point coordinates. the start point means when the animation starts, the point where the message box first appear. This value will always work no matter what animation style selected.
		* @param	uiStartOpacity		The opacity of the message box when the animation starts. The end opacity is the message box's opacity set in the skin.ini(normally 100). This value only work while AS_ALFA animation style selected.
		* @param	fStartScale			The scale of the messagebox when the animation starts. This scale value will act on both width and height. This value only work while AS_SCALE animation style selected.
		* @param	uiCircleCnt			The total circle count during the whole animation when messagebox shows.This value only work while AS_ROTATE animation style selected.
		* @param	bNeedDestroyAnimation	if need destroy animation, when messageBox destory, a converse animation of the show animation will play.
		*/
		void	setShowAnimationInfo(	UINT uiAnimationStyle = 0,
										UINT uiIntervalTime = 10, 
										UINT uiRepeatCount = 20,
										INT iStartX = -1, 
										INT iStartY = -1, 
										UINT uiStartOpacity = 100, 
										FLOAT fStartScale = 1.0f, 
										UINT uiCircleCnt = 0, 
										BOOLEAN bNeedDestroyAnimation = FALSE
									);

		/**
		* @brief	Set the message box parameter.
		*
		* @param	onOk					The callback function for OK button.
		* @param	pOKParam				The parameter for callback function of OK button.
		* @param	onCancel				The callback function for cancel button.
		* @param	pCanelParam				The parameter for callback function of cancel button.
		* @param	onAlternate				The callback function for Alternate button.
		* @param	pAlternateParam			The parameter for callback function of Alternate button.
		* @param	onDestroy				The callback function while destroy.
		* @param	pDestroyParam			The parameter for callback function while destroy.
		* @param	iAutoDisappearSecond	The time for auto disappear.
		*/
		void	doModal(PCALLBACK   onOk			= NULL,
						Object*		pOKParam		= NULL,
						PCALLBACK	onCancel		= NULL,
						Object*		pCanelParam		= NULL,
						PCALLBACK	onAlternate		= NULL,
						Object*     pAlternateParam = NULL,
						PCALLBACK	onDestroy		= NULL,
						Object*     pDestroyParam	= NULL,
						INT			iAutoDisappearSecond = 0);				

		/**
		* Set value to the default content.
		*/
		void			setContent(const String &strValue);
		const String&	getContent();

		/**
		* Set value to the named content, the name is defined in the OpenHMIArtist.
		*/
		void			setContent(const String &strName, const String &strValue);
		const String&	getContent(const String &strName) const;
		
		/**
		* Get the text field for default content.
		*/
		CCtrlTextField*	getContentTextField();

		/**
		* Get the text field for the named content, the name is defined in the OpenHMIArtist.
		*/
		CCtrlTextField*	 getContentTextField(const String &strName);
		
		/**
		* Get the image field for default icon.
		*/
		CCtrlImageField* getImageField();

		/**
		* Get the image field for the named icon, the name is defined in the OpenHMIArtist.
		*/
		CCtrlImageField* getImageField(const String &strName);

		/**
		*  Inner method.
		*/
		CCtrlTextField*	 extractTextField(const String& strName);

		/**
		*  Inner method.
		*/
		CCtrlImageField* extractImageField(const String& strName);

		/**
		*  Inner method.
		*/
		void			setRealWidth(UINT uiValue)	{ m_uiRealWidth = uiValue; }

		/**
		*  Inner method.
		*/
		void			setRealHeight(UINT uiValue)	{ m_uiRealHeight = uiValue; }

		CCtrlButton*	getButtonOK()			 { return &m_btnOk; }
		CCtrlButton*	getButtonCancel()		 { return &m_btnCancel; }
		CCtrlButton*	getButtonAlternate()	 { return &m_btnAlternate; }
		CCtrlTextField* getTitleTextField()	     { return &m_txtTitle; }

		virtual void    refresh();
		virtual	void	littleRefresh();
		virtual	void	destroy();
		
		virtual ICursorTracker* probe(INT iX, INT iY);

		virtual	void	buildRenderTree(CRenderGroup &renderGroup);

		// implement timer interface
		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);

	protected:// method
		CWgtMessageBox(const CWgtMessageBox&){}
		CWgtMessageBox& operator =(const CWgtMessageBox&){return *this;}

		virtual	void	buildRenderTreeFromBackground(CRenderGroup &renderGroup);

	private:// method
		void	onBtnOkTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);
		void	onBtnAlternateTchRel(CComponent *pTarget);

		void	startShowAnimation();
		void	doShowAnimation(UINT uiCurrentCount);
		void	completeShowAnimation();


		void	startHideAnimation();
		void	doHideAnimation(UINT uiCurrentCount);
		void	completeHideAnimaiton();
	protected:// property

	private:// property
		INT				m_iOffsetX;
		INT				m_iOffsetY;
		PCALLBACK		m_onOk;
		PCALLBACK		m_onCancel;
		PCALLBACK		m_onAlternate;
		PCALLBACK		m_onDestroy;
		INT				m_iOriginalX;
		INT				m_iOriginalY;

		Object*			m_pOKParam;
		Object*			m_pCancelParam;
		Object*			m_pAlternateParam;
		Object*			m_pDestroyParam;

		CCtrlTextField	m_txtTitle; 
		CCtrlButton		m_btnOk;
		CCtrlButton		m_btnCancel;
		CCtrlButton	    m_btnAlternate;

		Map<String, CCtrlTextField, tagStringCompare>  m_mapTextField;
		Map<String, CCtrlImageField, tagStringCompare> m_mapImageField;

		CTimer		m_timer;
		UINT		m_uiRealWidth;
		UINT		m_uiRealHeight;
		INT			m_iAutoDisappearSecond;

		// for animation
		BOOLEAN		m_bIsShowAnimationRunning;
		UINT		m_uiAnimationStyle;
		UINT		m_uiIntervalTime;
		UINT		m_uiRepeatCount;
		UINT		m_uiStartOpacity;
		UINT		m_uiCircleCnt;
		FLOAT		m_fStartScale;
		INT			m_iStartX;
		INT			m_iStartY;
		BOOLEAN		m_bNeedDestroyAnimation;
		BOOLEAN		m_bIsHideAnimationRunning;

		UINT		m_uiEndOpacity;
		FLOAT		m_fEndScale;
	};
}