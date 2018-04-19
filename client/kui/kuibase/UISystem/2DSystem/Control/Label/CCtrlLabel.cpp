#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlLabel::CCtrlLabel()
		: m_bLongFocus(FALSE)
	{
		setBaseView(&m_view);
		notAcceptFocus();
		setCanHitTest(FALSE);
	}

	CCtrlLabel::~CCtrlLabel()
	{
		;
	}

	void CCtrlLabel::setCaption(const String &strValue)
	{
		CCtrlTextField *pTextField = getTextField();
		if (pTextField != NULL)
		{
			pTextField->setContent(strValue);
		}
	}

	const String& CCtrlLabel::getCaption()
	{
		CCtrlTextField *pTextField = getTextField();
		if (pTextField != NULL)
		{
			return pTextField->getContent();
		}
		else
		{
			return STRING_NULL;
		}
	}

	void CCtrlLabel::setCaption(const String &strName, const String &strValue)
	{
		if( m_mapTextField.isExist(strName) )
		{
			m_mapTextField[strName]->setContent(strValue);
		}
	}

	const String& CCtrlLabel::getCaption(const String &strName) const
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

	CCtrlTextField* CCtrlLabel::getTextField()
	{
		return m_mapTextField.getValueByIndex(0);
	}

	CCtrlTextField* CCtrlLabel::getTextField(const String &strName)
	{
		return m_mapTextField[strName];
	}

	CCtrlImageField* CCtrlLabel::getImageField()
	{
		return m_mapImageField.getValueByIndex(0);
	}

	CCtrlImageField* CCtrlLabel::getImageField(const String &strName)
	{
		return m_mapImageField[strName];
	}

	CCtrlTextField* CCtrlLabel::extractTextField(const String &strName)
	{
		if( !m_mapTextField.isExist(strName) )
		{
			CCtrlTextField* pTextField = new CCtrlTextField();
			m_mapTextField.setItem(strName, pTextField);
			pTextField->setParent(this);

			return pTextField;
		}
		else
		{
			return m_mapTextField[strName];
		}
	}

	CCtrlImageField* CCtrlLabel::extractImageField(const String &strName)
	{
		if( !m_mapImageField.isExist(strName) )
		{
			CCtrlImageField* pImageField = new CCtrlImageField();
			m_mapImageField.setItem(strName, pImageField);
			pImageField->setParent(this);

			return pImageField;
		}
		else
		{
			return m_mapImageField[strName];
		}
	}

	void CCtrlLabel::refresh()
	{
		CComponent::refresh();

		updateView();

		m_mapTextField.resetIterator();
		while(m_mapTextField.hasMore())
		{
			m_mapTextField.next()->refresh();
		}

		m_mapImageField.resetIterator();
		while(m_mapImageField.hasMore())
		{
			m_mapImageField.next()->refresh();
		}
	}

	void CCtrlLabel::littleRefresh()
	{
		CComponent::littleRefresh();

		updateView();

		m_mapTextField.resetIterator();
		while(m_mapTextField.hasMore())
		{
			m_mapTextField.next()->littleRefresh();
		}

		m_mapImageField.resetIterator();
		while(m_mapImageField.hasMore())
		{
			m_mapImageField.next()->littleRefresh();
		}
	}

	void CCtrlLabel::destroySkin()
	{
		CComponent::destroySkin();

		m_mapTextField.resetIterator();
		while(m_mapTextField.hasMore())
		{
			m_mapTextField.next()->destroySkin();
		}

		m_mapImageField.resetIterator();
		while(m_mapImageField.hasMore())
		{
			m_mapImageField.next()->destroySkin();
		}
	}

	void CCtrlLabel::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlLabel::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (getVisible())
		{
			CPoint pt = localToGlobal(0, 0);
			m_renderGroup.setX(pt.getX());
			m_renderGroup.setY(pt.getY());
			m_renderGroup.setWidth(getPreferredSize().getX());
			m_renderGroup.setHeight(getPreferredSize().getY());
			renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

			buildRenderTreeFromBackground(m_renderGroup);

			m_mapImageField.resetIterator();
			while(m_mapImageField.hasMore())
			{
				m_mapImageField.next()->buildRenderTree(m_renderGroup);
			}

			m_mapTextField.resetIterator();
			while(m_mapTextField.hasMore())
			{
				m_mapTextField.next()->buildRenderTree(m_renderGroup);
			}

			buildRenderTreeFromCCP(m_renderGroup);
		}
	}

	void CCtrlLabel::updateView()
	{
		BOOLEAN	bCCPUseTouchFocus = FALSE;
		if (innerGetFocus())
		{
			if (m_CCPSkin.isNull())
			{
				m_view.drawSkin(m_skin, getEnable(), getHoverFocus(), TRUE, getSelected(), m_bLongFocus);
				bCCPUseTouchFocus = TRUE;
			}
			else
			{
				m_view.drawSkin(m_skin, getEnable(), getHoverFocus(), getTouchFocus(), getSelected(), m_bLongFocus);
			}
		}
		else
		{
			m_view.drawSkin(m_skin, getEnable(), getHoverFocus(), getTouchFocus(), getSelected(), m_bLongFocus);
		}

		m_mapTextField.resetIterator();
		while(m_mapTextField.hasMore())
		{
			CCtrlTextField* pTextField = m_mapTextField.next();

			if (pTextField->getFontCount() >= 5)
			{
				if (!getEnable())
				{
					pTextField->setActiveFont(3);
				}
				else
				{
					if (getSelected())
					{
						pTextField->setActiveFont(2);
					}
					else if (m_bLongFocus)
					{
						pTextField->setActiveFont(4);
					}
					else if (getTouchFocus() || bCCPUseTouchFocus)
					{
						pTextField->setActiveFont(1);
					}
					else
					{
						pTextField->setActiveFont(0);
					}
				}
			}
			else if (pTextField->getFontCount() == 4)
			{
				if (!getEnable())
				{
					pTextField->setActiveFont(3);
				}
				else
				{
					if (getSelected())
					{
						pTextField->setActiveFont(2);
					}
					else if (getTouchFocus() || bCCPUseTouchFocus || m_bLongFocus)
					{
						pTextField->setActiveFont(1);
					}
					else
					{
						pTextField->setActiveFont(0);
					}
				}
			}
			else if (pTextField->getFontCount() == 3)
			{
				if (!getEnable())
				{
					pTextField->setActiveFont(2);
				}
				else
				{
					if (getSelected())
					{
						pTextField->setActiveFont(2);
					}
					else if (getHoverFocus() || getTouchFocus() || bCCPUseTouchFocus || m_bLongFocus)
					{
						pTextField->setActiveFont(1);
					}
					else
					{
						pTextField->setActiveFont(0);
					}
				}
			}
			else
			{
				if (!getEnable())
				{
					pTextField->setActiveFont(0);
				}
				else
				{
					if (getHoverFocus() || getSelected() || getTouchFocus() || bCCPUseTouchFocus  || m_bLongFocus)
					{
						pTextField->setActiveFont(1);
					}
					else
					{
						pTextField->setActiveFont(0);
					}
				}
			}
		}
	}
}
