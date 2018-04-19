#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlTextField::CCtrlTextField()
		: m_pScrollbar(NULL)
		, m_bCircleScroll(FALSE)
		, m_bMultiLine(FALSE)
		, m_eWrapLine(CRenderText::WL_NOWRAP)
		, m_uiActiveFont(0)
		, m_bUseOmissions(FALSE)
		, m_strEndingForOmissions(L"...")
		, m_uiTextWidth(0)
		, m_uiTextHeight(0)
		, m_uiTextLineHeight(0)
		, m_bAutoHScroll(FALSE)
		, m_uiHFillerCount(0)
		, m_iHSpeed(0)
		, m_iHPauseTime(0)
		, m_eHScrollType(ST_CIRCLE)
		, m_iHOffsetStart(0)
		, m_iHOffsetEnd(0)
		, m_iHOffsetStep(0)
		, m_eHScrollState(SS_PREPARE)
		, m_bAutoVScroll(FALSE)
		, m_uiVFillerCount(0)		
		, m_iVSpeed(0)
		, m_iVPauseTime(0)		
		, m_iVOffsetStart(0)
		, m_iVOffsetEnd(0)
		, m_iVOffsetStep(0)		
		, m_eVScrollState(SS_PREPARE)
		, m_uiConstFontColor(0)
	{
		setCanHitTest(FALSE);
		notAcceptFocus();
		m_timerHS.setListener(this);
		m_timerVS.setListener(this);
	}

	CCtrlTextField::~CCtrlTextField()
	{
		m_timerHS.setListener(NULL);
		m_timerVS.setListener(NULL);
	}

	BEGIN_EVENT_MAP(CCtrlTextField)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CHANGED, onScrollbarPosChanged)
	END_EVENT_MAP_2

	void CCtrlTextField::setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bMovePageWhenClickUpDownArrow, BOOLEAN bCircleScroll)
	{
		if (m_pScrollbar == NULL)
		{
			m_pScrollbar = pScrollBar;
		}

		if (m_pScrollbar == NULL)
		{
			return;
		}

		m_bCircleScroll = bCircleScroll;

		m_pScrollbar->movePageWhenClickUpDownArrow(bMovePageWhenClickUpDownArrow);
		m_pScrollbar->addEventListener(this);

		resetScrollbar();
	}

	void CCtrlTextField::setVisible(BOOLEAN bValue)
	{
		CComponent::setVisible(bValue);

		contentPreprocess();
		resetScrollbar();

		setDirty();
	}

	void CCtrlTextField::setContent(const String &strValue)
	{
		if (m_strOrgContent != strValue)
		{
			m_strOrgContent = strValue;

			if ( m_bMultiLine )
			{
				m_renderText.setContent(m_strOrgContent);
			}
			else
			{
				m_renderText.setContent(chopWrap(m_strOrgContent));
			}

			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setHorizontalLayout(CRenderText::EHorizontalLayout eValue)
	{
		if (m_renderText.getHorizontalLayout() != eValue)
		{
			m_renderText.setHorizontalLayout(eValue);

			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setVerticalLayout(CRenderText::EVerticalLayout eValue)
	{
		if (m_renderText.getVerticalLayout() != eValue)
		{
			m_renderText.setVerticalLayout(eValue);

			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setBiDiMode(CRenderText::EBiDiMode eValue)
	{
		if (m_renderText.getBiDiMode() != eValue)
		{
			m_renderText.setBiDiMode(eValue);

			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setWrapLine(CRenderText::EWrapLine eValue)
	{
		m_eWrapLine = eValue;
		if (m_renderText.getWrapLine() != eValue)
		{
			if ( m_bMultiLine )
			{
				m_renderText.setWrapLine(eValue);
			}
			else
			{
				if ( m_renderText.getWrapLine() != CRenderText::WL_NOWRAP )
				{
					m_renderText.setWrapLine(CRenderText::WL_NOWRAP);
				}
				else
				{
					return;
				}
			}
			
			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setMultiLine(BOOLEAN bIsMultiLine)
	{
		if (m_bMultiLine != bIsMultiLine)
		{
			m_bMultiLine = bIsMultiLine;

			if( m_bMultiLine )
			{
				m_renderText.setContent(m_strOrgContent);
				m_renderText.setWrapLine(m_eWrapLine);
			}
			else
			{
				m_renderText.setContent(chopWrap(m_strOrgContent));
				m_renderText.setWrapLine(CRenderText::WL_NOWRAP);
			}

			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setLineSpacing(INT iValue)
	{
		if (iValue != getLineSpacing())
		{
			m_renderText.setLineSpacing(iValue);

			if (m_bMultiLine)
			{
				updateTextRenderInfo();				
				contentPreprocess();
				resetScrollbar();

				setDirty();
			}
		}
	}

	void CCtrlTextField::setFontName(const String &strValue)	
	{
		if (getFontName() != strValue)
		{
			m_renderText.getFont()->setName(strValue);

			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setFontColor(UINT uiValue)
	{
		if (m_uiConstFontColor != uiValue)
		{
			m_uiConstFontColor = uiValue;
			m_renderText.getFont()->setColor(uiValue); 
			setDirty();
		}
	}

	void CCtrlTextField::setFontSize(UINT uiValue)
	{
		if (getFontSize() != uiValue)
		{
			m_renderText.getFont()->setSize(uiValue); 

			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setFontStyle(CRenderFont::EFontStyle eValue)
	{
		if (getFontStyle() != eValue)
		{
			m_renderText.getFont()->setStyle(eValue); 

			updateTextRenderInfo();			
			contentPreprocess();
			resetScrollbar();

			setDirty();
		}
	}

	void CCtrlTextField::setActiveFont(UINT uiIndex)
	{
		if ( m_aryFontGroup.size() <= 0 )
		{
			return;
		}

		if ( uiIndex >= m_aryFontGroup.size() )
		{
			uiIndex = m_aryFontGroup.size() - 1;
		}

		m_renderText.getFont()->setName(m_aryFontGroup[uiIndex]->strFontName);
		if (m_uiConstFontColor != 0)
		{
			m_renderText.getFont()->setColor(m_uiConstFontColor);
		}
		else
		{
			m_renderText.getFont()->setColor(m_aryFontGroup[uiIndex]->uiColor);
		}
		m_renderText.getFont()->setSize(m_aryFontGroup[uiIndex]->uiSize); 
		m_renderText.getFont()->setStyle(m_aryFontGroup[uiIndex]->eFontStyle); 

		updateTextRenderInfo();		
		contentPreprocess();
		resetScrollbar();
		
		m_uiActiveFont = uiIndex;

		setDirty();
	}

	void CCtrlTextField::extractFontInfo(String strName, UINT uiColor, UINT uiSize,  CRenderFont::EFontStyle eFontStyle)
	{
		tagFontInfo* pFontInfo =  new tagFontInfo();
		pFontInfo->strFontName = strName;
		pFontInfo->uiColor = uiColor;
		pFontInfo->uiSize = uiSize;
		pFontInfo->eFontStyle = eFontStyle;

		m_aryFontGroup.addItem(pFontInfo);
	}

	void CCtrlTextField::setUseOmissions(BOOLEAN bValue)
	{
		if ( m_bUseOmissions == bValue )
		{
			return;
		}

		m_bUseOmissions = bValue;
		
		contentPreprocess();
		resetScrollbar();

		ENV::setDirty();
	}

	void CCtrlTextField::setEndingForOmissions(String strValue)
	{
		m_strEndingForOmissions = strValue;
		contentPreprocess();
	}

	void CCtrlTextField::setAutoHScroll(BOOLEAN bScroll, UINT uiPixelCnt, INT iSpeed, INT iPauseTime, EScrollType eScrollType)
	{
		m_bAutoHScroll = bScroll;
		m_uiHFillerCount = uiPixelCnt;
		m_iHSpeed = iSpeed;
		m_iHPauseTime = iPauseTime;
		m_eHScrollType = eScrollType;	

		if (m_bAutoHScroll)
		{
			calHScrollInfo();
			setFrequentChangeFlag(TRUE);

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->setIsAdjustMode(FALSE);
			}
		}
		else
		{
			m_renderText.setUseOffsetX(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerHS.stop();
			setFrequentChangeFlag(FALSE);

			resetScrollbar();

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->setIsAdjustMode(TRUE);
			}
		}
	}

	void CCtrlTextField::setAutoVScroll(BOOLEAN bScroll, UINT uiPixelCnt, INT iSpeed, INT iFirstLinePauseTime)
	{
		m_bAutoVScroll = bScroll;
		m_uiVFillerCount = uiPixelCnt;
		m_iVSpeed = iSpeed;
		m_iVPauseTime = iFirstLinePauseTime;		

		if (m_bAutoVScroll)
		{
			calVScrollInfo();
			setFrequentChangeFlag(TRUE);

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->setIsAdjustMode(FALSE);
			}
		}
		else
		{
			m_renderText.setUseOffsetY(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerVS.stop();
			setFrequentChangeFlag(FALSE);

			resetScrollbar();

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->setIsAdjustMode(TRUE);
			}
		}
	}	

	void CCtrlTextField::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		if ( pTimer == &m_timerHS )
		{
			switch (m_eHScrollState)
			{
			case SS_PAUSE:
				if ( m_bAutoHScroll )
				{
					if( m_uiTextWidth > (UINT)getPreferredSize().getX() )
					{
						setOffsetX(m_iHOffsetStart);
						m_eHScrollState = SS_SCROLLING;

						resetHTimer(m_iHSpeed);
					}
				}

				break;

			case SS_SCROLLING:
				if ( m_bAutoHScroll )
				{
					setOffsetX(m_renderText.getOffsetX() + m_iHOffsetStep);

					if( m_renderText.getOffsetX() == m_iHOffsetEnd )
					{
						m_eHScrollState = SS_PAUSE;
						setOffsetX(m_iHOffsetStart);

						resetHTimer(m_iHPauseTime);
					}
				}
				break;

			default:
				return;
			}
		}
		else
		{
			switch (m_eVScrollState)
			{
			case SS_PAUSE:
				if ( m_bAutoVScroll )
				{
					if ( m_uiTextHeight > (UINT)getPreferredSize().getY() )
					{
						setOffsetY( m_iVOffsetStart );

						m_eVScrollState = SS_SCROLLING;

						resetVTimer(m_iVSpeed);
					}
				}
				break;

			case SS_SCROLLING:
				if ( m_bAutoVScroll )
				{
					setOffsetY(m_renderText.getOffsetY() + m_iVOffsetStep);

					if ( m_renderText.getOffsetY() == m_iVOffsetEnd )
					{
						m_eVScrollState = SS_PAUSE;
						setOffsetY(m_iVOffsetStart);

						resetVTimer(m_iVPauseTime);
					}
				}
				break;

			default:
				return;
			}
		}
	}

	void CCtrlTextField::onTimerComplete(const CTimer *pTimer)
	{
		;
	}

	void CCtrlTextField::refresh()
	{
		CComponent::refresh();

		resetScrollbar();
	}

	void CCtrlTextField::littleRefresh()
	{
		CComponent::littleRefresh();

		resetScrollbar();
	}

	void CCtrlTextField::destroySkin()
	{
		CComponent::destroySkin();
		m_aryFontGroup.clear();
	}

	void CCtrlTextField::destroy()
	{
		m_timerHS.stop();
		m_timerVS.stop();

		CComponent::destroy();
	}

	void CCtrlTextField::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (getVisible() && getContent().length() > 0 && m_renderText.getFont()->getSize() > 0)
		{
			CPoint pt = localToGlobal(0, 0);
			m_renderGroup.setX(pt.getX());
			m_renderGroup.setY(pt.getY());
			m_renderGroup.setWidth(getPreferredSize().getX());
			m_renderGroup.setHeight(getPreferredSize().getY());
			renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

			m_renderText.setX(pt.getX());
			m_renderText.setY(pt.getY());
			m_renderText.setWidth(getPreferredSize().getX());
			m_renderText.setHeight(getPreferredSize().getY());

			m_renderGroup.addRenderItem(&m_renderText);
		}
	}

	String CCtrlTextField::chopWrap(const String &strValue)
	{
		String	strTemp = strValue;
		while (strTemp.indexOf(L"\n") >= 0)
		{
			strTemp.replaceFirst(L"\n",L"");
		}

		return strTemp;
	}

	void CCtrlTextField::updateTextRenderInfo()
	{
		if (getHasInitFromResource())
		{
			CRenderEngine::getInstance()->getTextInfo(&m_renderText, &m_uiTextWidth, &m_uiTextHeight, &m_uiTextLineHeight);			
		}
	}

	void CCtrlTextField::setOffsetX(INT iValue)
	{
		if ( m_renderText.getOffsetX() != iValue )
		{
			m_renderText.setOffsetX(iValue);
			setDirty();
		}
	}

	void CCtrlTextField::setOffsetY(INT iValue)
	{
		if ( m_renderText.getOffsetY() != iValue )
		{
			m_renderText.setOffsetY(iValue);

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->getModel()->setPosition(-iValue);
				m_pScrollbar->refresh();
			}

			setDirty();
		}
	}

	void CCtrlTextField::resetHTimer(INT iInterval)
	{
		m_timerHS.stop();
		m_timerHS.setDelay( iInterval );
		m_timerHS.setRepeatCount(0);
		m_timerHS.start();
	}

	void CCtrlTextField::resetVTimer(INT iInterval)
	{
		m_timerVS.stop();
		m_timerVS.setDelay( iInterval );
		m_timerVS.setRepeatCount(0);
		m_timerVS.start();
	}

	void CCtrlTextField::calHScrollInfo()
	{
		if ( !getVisible() )
		{
			return;
		}

		if (!m_bAutoHScroll)
		{
			m_renderText.setUseOffsetX(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerHS.stop();

			return;
		}

		if (!m_bMultiLine && m_bUseOmissions )
		{
			return;
		}

		if ( m_renderText.getHorizontalLayout() == CRenderText::HL_CENTER )
		{
			return;
		}

		if ( m_uiTextWidth < (UINT)getPreferredSize().getX() )
		{
			m_renderText.setUseOffsetX(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerHS.stop();

			return;
		}

		if (m_eHScrollType == ST_CIRCLE)
		{
			if( m_renderText.getBiDiMode() == CRenderText::BDM_LEFT_TO_RIGHT )
			{
				m_iHOffsetStart = 0;
				m_iHOffsetEnd = - ((INT)m_uiTextWidth + (INT)m_uiHFillerCount);
				m_iHOffsetStep = -1;
			}
			else
			{
				m_iHOffsetStart = (INT)getPreferredSize().getX() - (INT)m_uiTextWidth;
				m_iHOffsetEnd = (INT)getPreferredSize().getX() + (INT)m_uiHFillerCount;
				m_iHOffsetStep = 1;
			}
		}
		else if (m_eHScrollType == ST_REPEAT)
		{
			if( m_renderText.getBiDiMode() == CRenderText::BDM_LEFT_TO_RIGHT )
			{
				m_iHOffsetStart = 0;
				m_iHOffsetEnd = (INT)getPreferredSize().getX() - (INT)m_uiTextWidth - (INT)m_uiHFillerCount;
				m_iHOffsetStep = -1;
			}
			else
			{
				m_iHOffsetStart = (INT)getPreferredSize().getX() - (INT)m_uiTextWidth;
				m_iHOffsetEnd = 0 + m_uiHFillerCount;
				m_iHOffsetStep = 1;
			}
		}		

		if ( m_bAutoHScroll )
		{
			if (m_eHScrollType == ST_CIRCLE)
			{
				setOffsetX(m_iHOffsetStart);
				m_renderText.setUseOffsetX(TRUE);
				m_renderText.setHoritontalDuplicateSwitch(TRUE);
				m_renderText.setHoritontalDuplicateInterval(m_uiHFillerCount);
				m_eHScrollState = SS_PAUSE;
			}
			else if (m_eHScrollType == ST_REPEAT)
			{
				setOffsetX(m_iHOffsetStart);
				m_renderText.setUseOffsetX(TRUE);
				m_renderText.setHoritontalDuplicateSwitch(FALSE);
				m_eHScrollState = SS_PAUSE;
			}

			resetHTimer(m_iHPauseTime);
		}
	}

	void CCtrlTextField::calVScrollInfo()
	{
		if( !getVisible() )
		{
			return;
		}

		if (!m_bAutoVScroll)
		{
			m_renderText.setUseOffsetY(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerVS.stop();

			return;
		}

		if(!m_bMultiLine && m_bUseOmissions )
		{
			return;
		}

		if( m_renderText.getVerticalLayout() == CRenderText::VL_MIDDLE )
		{
			return;
		}

		if ( m_uiTextHeight < (UINT)getPreferredSize().getY() )
		{
			m_renderText.setUseOffsetY(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerVS.stop();

			return;
		}

		m_iVOffsetStart = 0;
		m_iVOffsetEnd = (UINT)getPreferredSize().getY() - m_uiTextHeight - m_uiVFillerCount;
		m_iVOffsetStep = -1;

		if( m_bAutoVScroll )
		{
			setOffsetY( m_iVOffsetStart );
			m_renderText.setUseOffsetY(TRUE);
			m_eVScrollState = SS_PAUSE;
			resetVTimer(m_iVPauseTime);
		}
	}

	void CCtrlTextField::formatEndingOmission()
	{
		if (m_bMultiLine || !m_bUseOmissions || m_renderText.getHorizontalLayout() == CRenderText::HL_CENTER )
		{
			return;
		}

		if ( m_bAutoHScroll )
		{
			m_renderText.setUseOffsetX(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerHS.stop();
			setFrequentChangeFlag(FALSE);
		}

		if ( m_bAutoVScroll )
		{
			m_renderText.setUseOffsetY(FALSE);
			m_renderText.setHoritontalDuplicateSwitch(FALSE);
			m_timerVS.stop();
			setFrequentChangeFlag(FALSE);
		}

		if ( m_uiTextWidth <= (UINT)getPreferredSize().getX() )
		{
			return;
		}

		String strOldContent = m_renderText.getContent();
		m_renderText.setContent(m_strEndingForOmissions);

		UINT uiEndingWidth = 0;
		UINT uiWidth = 0;
		UINT uiHeight = 0;
		UINT uiLineHeight = 0;
		
		CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiEndingWidth, &uiHeight, &uiLineHeight);

		if ( uiEndingWidth >= (UINT)getPreferredSize().getX() )
		{
			return;
		}
		
		UINT uiStart = 0;
		UINT uiEnd = strOldContent.length();
		UINT uiTemp = 0;

		if ( m_renderText.getHorizontalLayout() == CRenderText::HL_LEFT )
		{
			while(uiEnd - uiStart > 2)
			{
				uiTemp = uiStart + (uiEnd - uiStart)/ 2;

				m_renderText.setContent(strOldContent.subString(0, uiTemp));

				CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiWidth, &uiHeight, &uiLineHeight);
				if ( uiWidth > (UINT)getPreferredSize().getX() - uiEndingWidth )
				{
					uiEnd = uiTemp;
				}
				else
				{
					uiStart = uiTemp;
				}
				uiTemp = uiStart + (uiEnd - uiStart) / 2;
			}

			m_renderText.setContent(strOldContent.subString(0, uiTemp) + m_strEndingForOmissions);
			CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiWidth, &uiHeight, &uiLineHeight);

			while ( uiWidth > (UINT)getPreferredSize().getX() )
			{
				if( uiTemp > 1 )
				{
					m_renderText.setContent(strOldContent.subString(0, --uiTemp) + m_strEndingForOmissions);
					CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiWidth, &uiHeight, &uiLineHeight);
				}
				else
				{
					m_renderText.setContent(m_strEndingForOmissions);
					break;
				}
			}
		}
		else
		{
			while(uiEnd - uiStart > 2)
			{
				uiTemp = uiStart + (uiEnd - uiStart)/ 2;

				m_renderText.setContent(strOldContent.subString(uiTemp, strOldContent.length()));

				CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiWidth, &uiHeight, &uiLineHeight);
				if( uiWidth > (UINT)getPreferredSize().getX() - uiEndingWidth )
				{
					uiStart = uiTemp;
				}
				else
				{
					uiEnd = uiTemp;
				}
				uiTemp = uiStart + (uiEnd - uiStart) / 2;
			}

			m_renderText.setContent( m_strEndingForOmissions + strOldContent.subString(uiTemp, strOldContent.length()));
			CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiWidth, &uiHeight, &uiLineHeight);

			while( uiWidth > (UINT)getPreferredSize().getX() )
			{
				if( uiTemp < strOldContent.length() )
				{
					m_renderText.setContent(m_strEndingForOmissions + strOldContent.subString(++uiTemp, strOldContent.length()));
					CRenderEngine::getInstance()->getTextInfo(&m_renderText, &uiWidth, &uiHeight, &uiLineHeight);
				}
				else
				{
					m_renderText.setContent(m_strEndingForOmissions);
					break;
				}
			}
		}
	}

	void CCtrlTextField::contentPreprocess()
	{
		if ( m_bMultiLine )
		{
			m_renderText.setContent(m_strOrgContent);
		}
		else
		{
			m_renderText.setContent(chopWrap(m_strOrgContent));
		}

		calHScrollInfo();
		calVScrollInfo();
		formatEndingOmission();
	}

	void CCtrlTextField::resetScrollbar(BOOLEAN bMaintainPos)
	{
		if (m_pScrollbar == NULL)
		{
			return;
		}

		if (!m_bMultiLine)
		{
			m_pScrollbar->setVisible(FALSE);
			return;
		}		

		if (m_uiTextHeight <= (UINT)getPreferredSize().getY())
		{
			m_pScrollbar->setVisible(FALSE);
		}
		else
		{
			m_pScrollbar->setVisible(getVisible());
		}

		m_pScrollbar->getModel()->setScrollInfo(0, 
												m_uiTextHeight - 1, 
												bMaintainPos ? m_pScrollbar->getModel()->getPosition() : 0, 
												getPreferredSize().getY());
		m_pScrollbar->getModel()->setPreviousStep(m_uiTextLineHeight + m_renderText.getLineSpacing());
		m_pScrollbar->getModel()->setNextStep(m_uiTextLineHeight + m_renderText.getLineSpacing());
		m_pScrollbar->refresh();

		m_renderText.setUseOffsetY(TRUE);
		m_renderText.setOffsetY(0);
	}

	void CCtrlTextField::onScrollbarPosChanged(CComponent *pTarget)
	{
		if (!m_bMultiLine || m_bAutoHScroll || m_bAutoVScroll)
		{
			return;
		}

		INT iPos = m_pScrollbar->getModel()->getPosition();

		CCtrlScrollbar::EPosChanged eType = m_pScrollbar->getPosChangedType();

		if (iPos == 0)
		{
			if (eType == CCtrlScrollbar::PC_LINE_UP || eType == CCtrlScrollbar::PC_CONTINUE_LINE_UP)
			{
				if (m_renderText.getOffsetY() == 0 && m_bCircleScroll)
				{
					iPos = m_pScrollbar->getModel()->getMaxPosition();
					m_pScrollbar->getModel()->setPosition(iPos);
					m_pScrollbar->refresh();
				}
			}
		}
		else if (iPos == m_pScrollbar->getModel()->getMaxPosition())
		{
			if (eType == CCtrlScrollbar::PC_LINE_DOWN || eType == CCtrlScrollbar::PC_CONTINUE_LINE_DOWN)
			{
				if (-m_renderText.getOffsetY() == m_pScrollbar->getModel()->getMaxPosition() &&  m_bCircleScroll)
				{
					iPos = 0;
					m_pScrollbar->getModel()->setPosition(iPos);
					m_pScrollbar->refresh();
				}
			}
		}

		m_renderText.setOffsetY(-iPos);

		setDirty();
	}
}