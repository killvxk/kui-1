#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlButton::CCtrlButton()
		: m_bHandCursor(FALSE)
	{
		setAcceptFocus();
		setCanHitTest(TRUE);
		setCanDisabledTouch(TRUE);
	}

	CCtrlButton::~CCtrlButton()
	{
		;
	}

	void CCtrlButton::doFocusEnterPress()
	{
		if (getEnable())
		{
			setTouchFocus(TRUE);
			updateView();

			dispatchEvent(CEventButton::TOUCH_PRESSED);
		}
	}

	void CCtrlButton::doFocusEnterRelease()
	{
		if (getEnable())
		{
			setTouchFocus(FALSE);
			updateView();

			dispatchEvent(CEventButton::TOUCH_RELEASED);
		}
		else
		{
			dispatchEvent(CEventButton::DISABLE_TOUCHED);
		}
	}

	void CCtrlButton::onMove(INT iX, INT iY)
	{
		BOOLEAN bInInner = hitTest(iX, iY);

		if (getEnable())
		{
			if (bInInner)
			{
				if (m_bHandCursor == TRUE)
				{
					::SetClassLong(CScreenTouchServer::getInstance()->getScreenContext()->hWnd, GCL_HCURSOR,
						(LONG_PTR)LoadCursor(NULL, IDC_HAND));
				}
				else
				{
					::SetClassLong(CScreenTouchServer::getInstance()->getScreenContext()->hWnd, GCL_HCURSOR,
						(LONG_PTR)LoadCursor(NULL, IDC_ARROW));
				}

				if (getHoverFocus() == FALSE)
				{
					setHoverFocus(TRUE);
					updateView();
					dispatchEvent(CEventButton::HOVER_FOCUSED);
				}
			}
			else
			{

				if (getHoverFocus() == TRUE)
				{
					::SetClassLong(CScreenTouchServer::getInstance()->getScreenContext()->hWnd, GCL_HCURSOR,
						(LONG_PTR)LoadCursor(NULL, IDC_ARROW));
					setHoverFocus(FALSE);
					setTouchFocus(FALSE);
					updateView();
					dispatchEvent(CEventButton::HOVER_CANCELED);
				}
			}
		}
	}

	void CCtrlButton::onLostHover()
	{
		if (getHoverFocus() == FALSE)
			return;

		::SetClassLong(CScreenTouchServer::getInstance()->getScreenContext()->hWnd, GCL_HCURSOR,
			(LONG_PTR)LoadCursor(NULL, IDC_ARROW));
		setHoverFocus(FALSE);
		setTouchFocus(FALSE);
		updateView();
		dispatchEvent(CEventButton::HOVER_CANCELED);
	}

	void CCtrlButton::onTouchPress(INT iX, INT iY)
	{
		if (getEnable())
		{
			setTouchFocus(TRUE);
			m_bLongFocus = FALSE;
			updateView();

			dispatchEvent(CEventButton::TOUCH_PRESSED);
		}
	}

	void CCtrlButton::onTouchPressRelease(INT iX, INT iY)
	{
		if (getEnable())
		{
			setTouchFocus(FALSE);
			updateView();

			//ENV::switchCCPFocus(this);

			//ENV::playSoundForButtonClicked(this);
			dispatchEvent(CEventButton::TOUCH_RELEASED);
		}
		else
		{
			dispatchEvent(CEventButton::DISABLE_TOUCHED);
		}
	}

	void CCtrlButton::onTouchPressR(INT iX, INT iY)
	{
		if (getEnable())
		{
			dispatchEvent(CEventButton::TOUCH_PRESSED_R);
		}
	}

	void CCtrlButton::onTouchPressReleaseR(INT iX, INT iY)
	{
		if (getEnable())
		{
			dispatchEvent(CEventButton::TOUCH_RELEASED_R);
		}
	}

	void CCtrlButton::onTouchLongPress(INT iX, INT iY)
	{
		return;

		if (getEnable())
		{
			m_bLongFocus = TRUE;
			updateView();
			dispatchEvent(CEventButton::TOUCH_LONG_PRESSED);
		}
	}

	void CCtrlButton::onTouchContinuePress(INT iX, INT iY)
	{
		return;

		if (getEnable())
		{
			dispatchEvent(CEventButton::TOUCH_CONTINUE_PRESSED);
		}
	}

	void CCtrlButton::onTouchLongPressRelease(INT iX, INT iY)
	{
		return;

		if (getEnable())
		{
			setTouchFocus(FALSE);
			m_bLongFocus = FALSE;
			updateView();

			ENV::switchCCPFocus(this);

			ENV::playSoundForButtonClicked(this);

			dispatchEvent(CEventButton::TOUCH_LONG_RELEASED);
		}
	}

	void CCtrlButton::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		return;


		BOOLEAN bInInner = hitTest(iX, iY);

		if (getEnable())
		{
			if (bInInner)
			{
				if (getTouchFocus() == FALSE)
				{
					setTouchFocus(TRUE);
					updateView();
				}
			}
			else
			{
				if (getTouchFocus() == TRUE)
				{
					setTouchFocus(FALSE);
					updateView();
				}
			}
		}
	}

	void CCtrlButton::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		return;

		BOOLEAN bInInner = hitTest(iX, iY);

		if (getEnable())
		{
			setTouchFocus(FALSE);
			updateView();

			if (bInInner)
			{
				if (getParentMoving() == FALSE)
				{
					ENV::switchCCPFocus(this);

					ENV::playSoundForButtonClicked(this);

					dispatchEvent(CEventButton::TOUCH_RELEASED);
				}
				else
				{
					dispatchEvent(CEventButton::TOUCH_CANCELED);
				}
			}
			else
			{
				dispatchEvent(CEventButton::TOUCH_CANCELED);
			}
		}
		else
		{
			if (bInInner)
			{
				dispatchEvent(CEventButton::DISABLE_TOUCHED);
			}			
		}
	}

	void CCtrlButton::onTouchDragRelease(INT iX, INT iY)
	{
		return;
		if (getEnable())
		{
			setTouchFocus(FALSE);
			m_bLongFocus = FALSE;
			updateView();

			ENV::switchCCPFocus(this);
		}
	}

	void CCtrlButton::onTouchDoublePress(INT iX, INT iY)
	{
		return;

		if (getEnable())
		{
			setTouchFocus(TRUE);
			m_bLongFocus = FALSE;
			updateView();

			dispatchEvent(CEventButton::TOUCH_DOUBLE_PRESSED);
		}
	}

	void CCtrlButton::onTouchDoublePressRelease(INT iX, INT iY)
	{
		return;

		if (getEnable())
		{
			setTouchFocus(FALSE);
			updateView();

			ENV::switchCCPFocus(this);

			ENV::playSoundForButtonClicked(this);

			dispatchEvent(CEventButton::TOUCH_DOUBLE_RELEASED);
		}
	}
}
