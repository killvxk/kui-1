#include "OpenHMI.h"

namespace OpenHMI
{
	CWgtMessageBox::CWgtMessageBox()
		: m_iOffsetX(0)
		, m_iOffsetY(0)
		, m_onOk(NULL)
		, m_onCancel(NULL)
		, m_onAlternate(NULL)
		, m_onDestroy(NULL)
		, m_iOriginalX(-1)
		, m_iOriginalY(-1)
		, m_pOKParam(NULL)
		, m_pCancelParam(NULL)
		, m_pAlternateParam(NULL)
		, m_pDestroyParam(NULL)
		, m_uiRealWidth(0)
		, m_uiRealHeight(0)
		, m_iAutoDisappearSecond(0)
		, m_bIsShowAnimationRunning(FALSE)
		, m_uiAnimationStyle(0)
		, m_uiIntervalTime(10)
		, m_uiRepeatCount(20)
		, m_uiStartOpacity(100)
		, m_uiCircleCnt(0)
		, m_fStartScale(1.0f)
		, m_iStartX(-1)
		, m_iStartY(-1)
		, m_bNeedDestroyAnimation(FALSE)
		, m_bIsHideAnimationRunning(FALSE)
		, m_uiEndOpacity(100)
		, m_fEndScale(1.0f)
	{
		setType(CONFIG::CONTAINER_TYPE_CTRLWIDGET);

		m_timer.setListener(this);

		setPreferredSize(ENV::getScreenWidth(), ENV::getScreenHeight());

		addComponent(&m_txtTitle);

		m_btnOk.addEventListener(this);
		addComponent(&m_btnOk);

		m_btnCancel.addEventListener(this);
		addComponent(&m_btnCancel);

		m_btnAlternate.addEventListener(this);
		addComponent(&m_btnAlternate);
	}

	CWgtMessageBox::~CWgtMessageBox()
	{
	}

	BEGIN_EVENT_MAP(CWgtMessageBox)
		CONTROL_EVENT_HANDLER(&m_btnOk, CEventButton::TOUCH_RELEASED, onBtnOkTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		CONTROL_EVENT_HANDLER(&m_btnAlternate, CEventButton::TOUCH_RELEASED, onBtnAlternateTchRel)
	END_EVENT_MAP(CContainer)

	void CWgtMessageBox::setParam(	const String& strTitle, 
									const String& strContent, 
									const String& strOk, 
									const String& strCancel, 
									const String& strAlternate,
									INT	   ibtnOkSoundType,
									INT	   ibtnCancelSoundType,
									INT    ibtnAlternateSoundType)
	{
		if (strTitle != STRING_NULL)
		{
			m_txtTitle.setContent(strTitle);
		}
		
		if (strContent != STRING_NULL)
		{
			setContent(strContent);
		}		

		if (strOk != STRING_NULL)
		{
			m_btnOk.setCaption(strOk);
		}
		m_btnOk.setSoundType(ibtnOkSoundType);

		if (strCancel != STRING_NULL)
		{
			m_btnCancel.setCaption(strCancel);
		}		
		m_btnCancel.setSoundType(ibtnCancelSoundType);

		if (strAlternate != STRING_NULL)
		{
			m_btnAlternate.setCaption(strAlternate);
		}		
		m_btnAlternate.setSoundType(ibtnAlternateSoundType);
	}

	void CWgtMessageBox::setShowAnimationInfo(	UINT uiAnimationStyle,
												UINT uiIntervalTime, 
												UINT uiRepeatCount,
												INT iStartX, 
												INT iStartY, 
												UINT uiStartOpacity, 
												FLOAT fStartScale, 
												UINT uiCircleCnt, 
												BOOLEAN bNeedDestroyAnimation
											)
	{
		m_uiAnimationStyle = uiAnimationStyle;
		m_uiIntervalTime = uiIntervalTime;
		m_uiRepeatCount = uiRepeatCount;
		m_uiStartOpacity = uiStartOpacity;
		m_uiCircleCnt = uiCircleCnt;
		m_fStartScale = fStartScale;
		m_iStartX = iStartX;
		m_iStartY = iStartY;
		m_bNeedDestroyAnimation = bNeedDestroyAnimation;
	}

	void CWgtMessageBox::doModal(	PCALLBACK	onOk,
									Object*		pOKParam,
									PCALLBACK	onCancel,
									Object*		pCanelParam,
									PCALLBACK	onAlternate,
									Object*     pAlternateParam,
									PCALLBACK	onDestroy,
									Object*     pDestroyParam,
									INT			iAutoDisappearSecond)
	{
		m_onOk			= onOk;
		m_onCancel		= onCancel;
		m_onAlternate	= onAlternate;
		m_onDestroy     = onDestroy;

		m_pOKParam		  = pOKParam;
		m_pCancelParam    = pCanelParam;
		m_pAlternateParam = pAlternateParam;
		m_pDestroyParam   = pDestroyParam;
		m_iAutoDisappearSecond = iAutoDisappearSecond;

		m_iOffsetX = (getPreferredSize().getX() - (INT)m_uiRealWidth) / 2;
		m_iOffsetY = (getPreferredSize().getY() - (INT)m_uiRealHeight) / 2;

		refresh();

		startShowAnimation();
	}	

	void CWgtMessageBox::setContent(const String& strValue)
	{
		CCtrlTextField *pTextField = getContentTextField();
		if (pTextField != NULL)
		{
			pTextField->setContent(strValue);
		}
	}

	const String& CWgtMessageBox::getContent()
	{
		CCtrlTextField *pTextField = getContentTextField();
		if (pTextField != NULL)
		{
			return pTextField->getContent();
		}
		else
		{
			return STRING_NULL;
		}
	}

	void CWgtMessageBox::setContent(const String &strName, const String &strValue)
	{
		if( m_mapTextField.isExist(strName) )
		{
			m_mapTextField[strName]->setContent(strValue);
		}
	}

	const String& CWgtMessageBox::getContent(const String &strName) const
	{
		if( m_mapTextField.isExist(strName) )
		{
			return m_mapTextField[strName]->getContent();
		}
		else
		{
			return STRING_NULL;
		}
	}

	CCtrlTextField* CWgtMessageBox::getContentTextField()
	{
		return m_mapTextField.getValueByIndex(0);
	}

	CCtrlTextField* CWgtMessageBox::getContentTextField(const String &strName)
	{
		return m_mapTextField[strName];
	}

	CCtrlImageField* CWgtMessageBox::getImageField()
	{
		return m_mapImageField.getValueByIndex(0);
	}

	CCtrlImageField* CWgtMessageBox::getImageField(const String &strName)
	{
		return m_mapImageField[strName];
	}

	CCtrlTextField* CWgtMessageBox::extractTextField(const String& strName)
	{
		if( !m_mapTextField.isExist(strName) )
		{
			CCtrlTextField* pTextField = new CCtrlTextField();
			m_mapTextField.setItem(strName, pTextField);
			addComponent(pTextField);

			return pTextField;
		}
		else
		{
			return m_mapTextField[strName];
		}
	}

	CCtrlImageField* CWgtMessageBox::extractImageField(const String& strName)
	{
		if( !m_mapImageField.isExist(strName) )
		{
			CCtrlImageField* pImageField = new CCtrlImageField();
			m_mapImageField.setItem(strName, pImageField);
			addComponent(pImageField);

			return pImageField;
		}
		else
		{
			return m_mapImageField[strName];
		}
	}

	void CWgtMessageBox::refresh()
	{
		CContainer::refresh();

		// Button visible
		if (m_btnOk.getPreferredSize().getX() == 0 || m_btnOk.getPreferredSize().getY() == 0)
		{
			m_btnOk.setVisible(FALSE);
		}

		if (m_btnCancel.getPreferredSize().getX() == 0 || m_btnCancel.getPreferredSize().getY() == 0)
		{
			m_btnCancel.setVisible(FALSE);
		}

		if (m_btnAlternate.getPreferredSize().getX() == 0 || m_btnAlternate.getPreferredSize().getY() == 0)
		{
			m_btnAlternate.setVisible(FALSE);
		}

		m_iOriginalX = m_iOriginalX == -1 ? m_iOffsetX : m_iOriginalX;
		m_iOriginalY = m_iOriginalY == -1 ? m_iOffsetY : m_iOriginalY;
	}

	void CWgtMessageBox::littleRefresh()
	{
		refresh();
	}

	void CWgtMessageBox::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		if( m_timer.isRunning() )
		{
			completeHideAnimaiton();
		}
		else
		{
			startHideAnimation();
		}
	}

	ICursorTracker* CWgtMessageBox::probe(INT iX, INT iY)
	{
		ICursorTracker* pTracker = CContainer::probe(iX, iY);
		
		if (pTracker != NULL)
		{
			return pTracker;
		}
		else
		{
			if (   0 <= iX && iX <= getPreferredSize().getX() + (INT)ENV::getHitOffset() * 2
				&& 0 <= iY && iY <= getPreferredSize().getY() + (INT)ENV::getHitOffset() * 2   )
			{
				return this;
			}
			
			return NULL;
		}
	}

	void CWgtMessageBox::buildRenderTree(CRenderGroup &renderGroup)
	{
		setX(0);
		setY(0);
		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX() + m_iOffsetX);
		m_renderGroup.setY(pt.getY() + m_iOffsetY);
		m_renderGroup.setWidth(m_uiRealWidth);
		m_renderGroup.setHeight(m_uiRealHeight);
		renderGroup.addRenderItem(&m_renderGroup);

		setX(m_iOffsetX);
		setY(m_iOffsetY);
		buildRenderTreeFromBackground(m_renderGroup);

		buildRenderTreeFromChildren(m_renderGroup);		

		buildRenderTreeFromCCP(m_renderGroup);
	}

	void CWgtMessageBox::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		if( m_bIsShowAnimationRunning )
		{
			doShowAnimation(uiCurrentCount);
		}
		else if( m_bIsHideAnimationRunning )
		{
			doHideAnimation(uiCurrentCount);
		}
	}

	void CWgtMessageBox::onTimerComplete(const CTimer *pTimer)
	{
		if( m_bIsShowAnimationRunning )
		{
			completeShowAnimation();
		}
		else if( m_bIsHideAnimationRunning )
		{
			completeHideAnimaiton();
		}
		else
		{
			if (m_onCancel != NULL)
			{
				m_onCancel(m_pCancelParam);
			}
			else if (m_onOk != NULL)
			{
				m_onOk(m_pOKParam);
			}
			else if (m_onAlternate != NULL)
			{
				m_onAlternate(m_pAlternateParam);
			}		

			destroy();
		}
	}

	void CWgtMessageBox::buildRenderTreeFromBackground(CRenderGroup &renderGroup)
	{
		if (getVisible() && m_skin.isNotNull())
		{
			m_renderImage.setFilePath(m_skin->getCurrentImage());

			CPoint pt = localToGlobal(0, 0);

			m_renderImage.setX(pt.getX());
			m_renderImage.setY(pt.getY());
			m_renderImage.setWidth(m_uiRealWidth);
			m_renderImage.setHeight(m_uiRealHeight);

			m_renderImage.setLayout(CRenderImage::L_NORMAL);

			renderGroup.addRenderItem(&m_renderImage);
		}
	}

	void CWgtMessageBox::onBtnOkTchRel(CComponent *pTarget)
	{
		if (m_onOk != NULL)
		{
			m_onOk(m_pOKParam);
		}

		destroy();
	}

	void CWgtMessageBox::onBtnCancelTchRel(CComponent *pTarget)
	{
		if (m_onCancel != NULL)
		{
			m_onCancel(m_pCancelParam);
		}

		destroy();
	}

	void CWgtMessageBox::onBtnAlternateTchRel(CComponent *pTarget)
	{
		if (m_onAlternate != NULL)
		{
			m_onAlternate(m_pAlternateParam);
		}

		destroy();
	}

	void CWgtMessageBox::startShowAnimation()
	{
		lock();

		m_uiEndOpacity = getOpacity();
		m_fEndScale = getScaleWidth();

		if( m_uiIntervalTime > 0 && m_uiRepeatCount > 0 )
		{
			if( m_uiAnimationStyle == 0 )
			{
				completeShowAnimation();
			}

			if( ( m_uiAnimationStyle & AS_ALFA ) == AS_ALFA)
			{
				setOpacity(m_uiStartOpacity);
			}
			
			if( ( m_uiAnimationStyle & AS_ROTATE) == AS_ROTATE )
			{
				;
			}
			
			if( ( m_uiAnimationStyle & AS_SCALE ) == AS_SCALE )
			{
				setScaleWidth(m_fStartScale);
				setScaleHeight(m_fStartScale);
			}

			m_iOffsetX = m_iStartX == -1 ? m_iOffsetX : m_iStartX;
			m_iOffsetY = m_iStartY == -1 ? m_iOffsetY : m_iStartY;

			m_timer.stop();
			m_timer.setDelay(m_uiIntervalTime);
			m_timer.setRepeatCount(m_uiRepeatCount);
			m_timer.start();
			m_bIsShowAnimationRunning = TRUE;
		}
		else
		{
			completeShowAnimation();
		}
	}

	void CWgtMessageBox::doShowAnimation(UINT uiCurrentCount)
	{
		if( ( m_uiAnimationStyle & AS_ALFA ) == AS_ALFA )
		{
			setOpacity( m_uiStartOpacity + ( m_uiEndOpacity - m_uiStartOpacity ) * uiCurrentCount / m_uiRepeatCount);
		}
		
		if( ( m_uiAnimationStyle & AS_ROTATE ) == AS_ROTATE )
		{
			setRotateAngle( FLOAT((360 * m_uiCircleCnt) * uiCurrentCount)/ m_uiRepeatCount, INT(m_uiRealWidth / 2), INT(m_uiRealHeight / 2), CRenderGroup::RA_Z);
		}
		
		if( ( m_uiAnimationStyle & AS_SCALE ) == AS_SCALE)
		{
			setScaleWidth( m_fStartScale + ( m_fEndScale - m_fStartScale ) * uiCurrentCount / m_uiRepeatCount);
			setScaleHeight( getScaleWidth() );
		}

		if( m_iStartX != -1 )
		{
			m_iOffsetX = m_iStartX + ((getPreferredSize().getX() - (INT)m_uiRealWidth) / 2 - m_iStartX) * (INT)uiCurrentCount / (INT)m_uiRepeatCount;
		}

		if( m_iStartY != -1 )
		{
			m_iOffsetY = m_iStartY + ((getPreferredSize().getY() - (INT)m_uiRealHeight) / 2 - m_iStartY) * (INT)uiCurrentCount / (INT)m_uiRepeatCount;
		}
	}

	void CWgtMessageBox::completeShowAnimation()
	{
		unlock();

		m_bIsShowAnimationRunning = FALSE;
		m_iOffsetX = m_iOriginalX;
		m_iOffsetY = m_iOriginalY;

		setOpacity(m_uiEndOpacity);
		setScaleWidth( m_fEndScale);
		setScaleHeight( m_fEndScale );
		setRotateAngle(0.0f, INT(m_uiRealWidth / 2), INT(m_uiRealHeight / 2),CRenderGroup::RA_Z);

		if (m_iAutoDisappearSecond > 0)
		{	
			m_timer.stop();
			m_timer.setRepeatCount(1);
			m_timer.setDelay(m_iAutoDisappearSecond * 1000);
			m_timer.start();
		}

		if (CONFIG::getUseCCP())
		{
			setFocus();
		}
	}

	void CWgtMessageBox::startHideAnimation()
	{
		lock();

		if( !m_bNeedDestroyAnimation )
		{
			completeHideAnimaiton();
			return;
		}

		m_uiEndOpacity = getOpacity();
		m_fEndScale = getScaleWidth();

		if( m_uiIntervalTime > 0 && m_uiRepeatCount > 0 )
		{
			if( m_uiAnimationStyle == 0 )
			{
				completeShowAnimation();
			}

			if( ( m_uiAnimationStyle & AS_ALFA ) == AS_ALFA)
			{
				setOpacity(m_uiEndOpacity);
			}
			
			if( ( m_uiAnimationStyle & AS_ROTATE) == AS_ROTATE )
			{
				;
			}
			
			if( ( m_uiAnimationStyle & AS_SCALE ) == AS_SCALE )
			{
				setScaleWidth(m_fEndScale);
				setScaleHeight(m_fEndScale);
			}

			m_iOffsetX = m_iOriginalX;
			m_iOffsetY = m_iOriginalY;

			m_timer.stop();
			m_timer.setDelay(m_uiIntervalTime);
			m_timer.setRepeatCount(m_uiRepeatCount);
			m_timer.start();
			m_bIsHideAnimationRunning = TRUE;
		}
		else
		{
			completeHideAnimaiton();
		}
	}

	void CWgtMessageBox::doHideAnimation(UINT uiCurrentCount)
	{
		if( ( m_uiAnimationStyle & AS_ALFA ) == AS_ALFA )
		{
			setOpacity( m_uiEndOpacity + ( m_uiStartOpacity - m_uiEndOpacity ) * uiCurrentCount / m_uiRepeatCount);
		}
		
		if( ( m_uiAnimationStyle & AS_ROTATE ) == AS_ROTATE )
		{
			setRotateAngle( FLOAT((360 * m_uiCircleCnt) * uiCurrentCount)/ m_uiRepeatCount, INT(m_uiRealWidth / 2), INT(m_uiRealHeight / 2), CRenderGroup::RA_Z);
		}
		
		if( ( m_uiAnimationStyle & AS_SCALE ) == AS_SCALE)
		{
			setScaleWidth( m_fEndScale + ( m_fStartScale - m_fEndScale ) * uiCurrentCount / m_uiRepeatCount);
			setScaleHeight( getScaleWidth() );
		}

		if( m_iStartX != -1 )
		{
			m_iOffsetX = (getPreferredSize().getX() - (INT)m_uiRealWidth) / 2 + ( m_iStartX - (getPreferredSize().getX() - (INT)m_uiRealWidth) / 2 ) * (INT)uiCurrentCount / (INT)m_uiRepeatCount;
		}

		if( m_iStartY != -1 )
		{
			m_iOffsetY = (getPreferredSize().getY() - (INT)m_uiRealHeight) / 2 + ( m_iStartY - (getPreferredSize().getY() - (INT)m_uiRealHeight) / 2 ) * (INT)uiCurrentCount / (INT)m_uiRepeatCount;
		}
	}

	void CWgtMessageBox::completeHideAnimaiton()
	{
		unlock();

		m_onOk = NULL;
		m_onCancel = NULL;
		m_onAlternate = NULL;

		m_pOKParam		  = NULL;
		m_pCancelParam    = NULL;
		m_pAlternateParam = NULL;

		// delete self content
		m_timer.setListener(NULL);
		m_timer.stop();

		m_btnOk.removeEventListener(this);
		m_btnCancel.removeEventListener(this);
		m_btnAlternate.removeEventListener(this);

		// Callback to user destroy
		if (m_onDestroy != NULL)
		{
			m_onDestroy(this);
			m_onDestroy = NULL;
			m_pDestroyParam = NULL;
		}

		CContainer::destroy();

		m_mapImageField.clear();
		m_mapTextField.clear();

		delete this;
	}
}