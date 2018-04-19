/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CCtrlTextField
			: public CComponent
			, public IEventListener
			, public ITimerListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EScrollType
		{
			ST_CIRCLE			= 0,
			ST_REPEAT			= 1,
		};

	private:// embed class or struct or enum
		enum EScrollState
		{
			SS_PREPARE			= 0,
			SS_PAUSE			= 1,
			SS_SCROLLING		= 2,
		};

		struct tagFontInfo
		{
			String						strFontName;
			UINT						uiColor;
			UINT						uiSize;
			CRenderFont::EFontStyle		eFontStyle;

			tagFontInfo()
				: strFontName(STRING_NULL)
				, uiColor(0)
				, uiSize(0)
				, eFontStyle(CRenderFont::FS_REGULAR)
			{
				;
			}
		};

	public:// method
		CCtrlTextField();
		virtual ~CCtrlTextField();

		DECLARE_EVENT_MAP

		/**
		* @brief	Associate scroll bar with this control.
		*			Usually scrollbar is always shown if the control can be scrolled, and it is hidden if the control can't be scrolled.
		*
		* @param	bMovePageWhenClickUpDownArrow	If it is TRUE, when clicking the up or down arrow of the scroll bar, it will turn over the total height of the control.
		*											If it is FALSE, when clicking the up or down arrow of the scroll bar, it will turn over one line height of the control.
		* @param	bCircleScroll					If it is TRUE, when it shows the first page and then click the top arrow again, it will show the last page. 
		*/
		void							setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bMovePageWhenClickUpDownArrow = TRUE, BOOLEAN bCircleScroll = FALSE);


		virtual void					setVisible(BOOLEAN bValue);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void							setContent(const String &strValue);
		const String&					getContent() const			{ return m_strOrgContent; }		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void							setHorizontalLayout(CRenderText::EHorizontalLayout eValue);
		CRenderText::EHorizontalLayout	getHorizontalLayout() const	{ return  m_renderText.getHorizontalLayout(); }
		void							setVerticalLayout(CRenderText::EVerticalLayout eValue);
		CRenderText::EVerticalLayout	getVerticalLayout() const	{ return m_renderText.getVerticalLayout(); }
		void							setBiDiMode(CRenderText::EBiDiMode eValue);
		CRenderText::EBiDiMode			getBiDiMode() const			{ return m_renderText.getBiDiMode(); }
		void							setWrapLine(CRenderText::EWrapLine eValue);
		CRenderText::EWrapLine			getWrapLine() const			{ return m_renderText.getWrapLine(); }
		void							setMultiLine(BOOLEAN bIsMultiLine);
		BOOLEAN							getMultiLine() const		{ return m_bMultiLine; }
		void							setLineSpacing(INT iValue);
		INT								getLineSpacing() const		{ return m_renderText.getLineSpacing(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void							setFontName(const String &strValue);
		const String&					getFontName() const			{ return m_renderText.getFont()->getName(); }
		void							setFontColor(UINT uiValue);
		UINT							getFontColor() const		{ return m_renderText.getFont()->getColor(); }
		void							setFontSize(UINT uiValue);
		UINT							getFontSize() const			{ return m_renderText.getFont()->getSize(); }
		void							setFontStyle(CRenderFont::EFontStyle eValue);
		CRenderFont::EFontStyle			getFontStyle() const		{ return m_renderText.getFont()->getStyle(); }

		/**
		* @brief	Active font means the effective font in the font list.
		*			A text field has a font list and you can do font animation by this way.
		*
		* @param	uiIndex The index to the active font.
		*/
		void							setActiveFont(UINT uiIndex);
		UINT							getActiveFont()	const		{ return m_uiActiveFont; }
		UINT							getFontCount() const		{ return m_aryFontGroup.size(); }		

		/**
		*  Inner method.
		*/
		void							extractFontInfo(String strName, UINT uiColor, UINT uiSize, CRenderFont::EFontStyle eFontStyle);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Set whether the content is shown with an ending string indicates omissions or not.
		*			When the text field is single line and the content is longer than the text field's width,
		*			there are two ways to tell the user that the content is not shown completely.
		*			The first way is to scroll content.
		*			The second way is to show content with an ending string indicates omissions.
		*
		* @param	bValue	TRUE	The content is shown with an ending string indicates omissions.
		*					FALSE	The content is shown without an ending string indicates omissions.
		*/
		void							setUseOmissions(BOOLEAN bValue);

		BOOLEAN							getUseOmissions()					{ return m_bUseOmissions; }

		/**
		* @brief	Set an ending string indicates omissions. The default string is "...".
		*
		* @param	strValue The ending string indicates omissions.
		*/
		void							setEndingForOmissions(String strValue);

		/**
		 * @brief	When the text is single line, you can make it auto scroll when the rendering width is larger than the text field width.
		 *
		 * @param	bScroll		Indicate whether scroll or not.
		 * @param	uiPixelCnt	The pixel you want to keep scrolling when scroll the end of text field
		 * @param	iSpeed		Indicate the speed for each pixel moves.The unit is millisecond.
		 * @param	iPauseTime	It will pause this interval when begin to scroll.The unit is millisecond
		 * @param	eScrollType	Select horitonal scrool type.
		 */
		void							setAutoHScroll(BOOLEAN bScroll, UINT uiPixelCnt = 0, INT iSpeed = 25, INT iPauseTime = 2000, EScrollType eScrollType = ST_CIRCLE);

		/**
		 * @brief	When the text is multi-line, you can make it auto scroll when the rendering height is larger than the text field height.
		 *
		 * @param	bScroll		Indicate whether scroll or not.
		 * @param	uiPixelCnt	The pixel you want to keep scrolling when scroll the end of text field.
		 * @param	iSpeed		Indicate the speed for each pixel moves.The unit is millisecond.
		 * @param	iPauseTime	It will pause this interval when begin to scroll. The unit is millisecond
		 */
		void							setAutoVScroll(BOOLEAN bScroll, UINT uiPixelCnt = 0, INT iSpeed = 25, INT iPauseTime = 2000);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// implement timer interface
		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);

		virtual	void	refresh();
		virtual	void	littleRefresh();
		virtual void	destroySkin();
		virtual	void	destroy();	
		virtual void	buildRenderTree(CRenderGroup &renderGroup);


	protected:// method
		CCtrlTextField(const CCtrlTextField&){}
		CCtrlTextField& operator =(const CCtrlTextField&){return *this;}

	private:// method
		String			chopWrap(const String &strValue);

		void			updateTextRenderInfo();

		void			setOffsetX(INT iValue);
		void			setOffsetY(INT iValue);
		void			resetHTimer(INT iInterval);
		void			resetVTimer(INT iInterval);
		void			calHScrollInfo();
		void			calVScrollInfo();

		void			formatEndingOmission();
		void			contentPreprocess();

		void			resetScrollbar(BOOLEAN bMaintainPos = FALSE);
		void			onScrollbarPosChanged(CComponent *pTarget);

	protected:// property

	private:// property
		CRenderText						m_renderText;

		CCtrlScrollbar*					m_pScrollbar;
		BOOLEAN							m_bCircleScroll;

		BOOLEAN							m_bMultiLine;
		CRenderText::EWrapLine			m_eWrapLine;
		String							m_strOrgContent;
		
		Array<tagFontInfo>				m_aryFontGroup;
		UINT							m_uiActiveFont;	

		BOOLEAN							m_bUseOmissions;
		String							m_strEndingForOmissions;

		UINT							m_uiTextWidth;
		UINT							m_uiTextHeight;
		UINT							m_uiTextLineHeight;

		BOOLEAN							m_bAutoHScroll;
		UINT							m_uiHFillerCount;
		INT								m_iHSpeed;
		INT								m_iHPauseTime;
		EScrollType						m_eHScrollType;
		INT								m_iHOffsetStart;
		INT								m_iHOffsetEnd;
		INT								m_iHOffsetStep;
		EScrollState					m_eHScrollState;
		CTimer							m_timerHS;
		
		BOOLEAN							m_bAutoVScroll;	
		UINT							m_uiVFillerCount;
		INT								m_iVSpeed;
		INT								m_iVPauseTime;
		INT								m_iVOffsetStart;
		INT								m_iVOffsetEnd;
		INT								m_iVOffsetStep;				
		EScrollState					m_eVScrollState;
		CTimer							m_timerVS;

		UINT							m_uiConstFontColor;
	};
}