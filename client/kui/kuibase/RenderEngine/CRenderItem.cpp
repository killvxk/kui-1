#include "OpenHMI.h"

namespace OpenHMI
{
	CRenderItem::CRenderItem()
		: m_eType(RIT_UNKNOWN)		
		, m_pParent(NULL)
		, m_iX(0)
		, m_iY(0)
		, m_uiWidth(0)
		, m_uiHeight(0)		
	{
		;
	}

	CRenderItem::~CRenderItem()
	{
		if ( m_ptrCacheData != NULL )
		{
			m_ptrCacheData = NULL;
		}
	}

	void CRenderItem::setX(INT iValue)
	{
		if ( iValue == m_iX )
		{
			return;
		}

		m_iX = iValue;


		notifyMergePropertyChanged();
	}

	void CRenderItem::setY(INT iValue)
	{
		if ( iValue == m_iY )
		{
			return;
		}

		m_iY = iValue;

		notifyMergePropertyChanged();
	}

	void CRenderItem::setWidth(UINT uiValue)
	{
		if ( uiValue == m_uiWidth )
		{
			return;
		}

		m_uiWidth = uiValue;

		notifySizePropertyChanged();
	}

	void CRenderItem::setHeight(UINT uiValue)
	{
		if ( uiValue == m_uiHeight )
		{
			return;
		}

		m_uiHeight = uiValue;

		notifySizePropertyChanged();
	}

	CRenderItem* CRenderItem::hitTest(INT iX, INT iY)
	{
		INT iTempX = m_uiWidth <= 3 ? m_iX - 3 : m_iX;
		INT iTempY = m_uiHeight <= 3 ? m_iY - 3 : m_iY;
		UINT uiTempWidth = m_uiWidth <= 3 ? m_uiWidth + 6 : m_uiWidth;
		UINT uiTempHeight = m_uiHeight <= 3 ? m_uiHeight + 6 : m_uiHeight;

		if (   iTempX <= iX && iX <= iTempX + (INT)(uiTempWidth)
			&& iTempY <= iY && iY <= iTempY + (INT)(uiTempHeight)   )
		{
			return this;
		}
		else
		{
			return NULL;
		}
	}

	void CRenderItem::notifySizePropertyChanged()
	{
		if ( m_ptrCacheData != NULL )
		{
			m_ptrCacheData->notifySizePropertyChanged();
		}

		if (m_pParent != NULL)
		{
			m_pParent->notifyChildrenChanged();
		}
	}

	void CRenderItem::notifyMergePropertyChanged()
	{
		if ( m_ptrCacheData != NULL )
		{
			m_ptrCacheData->notifyMergePropertyChanged();
		}

		if (m_pParent != NULL)
		{
			m_pParent->notifyChildrenChanged();
		}
	}

	void CRenderItem::notifyContentPropertyChanged()
	{
		if ( m_ptrCacheData != NULL )
		{
			m_ptrCacheData->notifyContentPropertyChanged();
		}

		if (m_pParent != NULL)
		{
			m_pParent->notifyChildrenChanged();
		}
	}

	void CRenderItem::notifyChildrenChanged()
	{
		if ( m_ptrCacheData != NULL )
		{
			m_ptrCacheData->notifyChildrenChanged();
		}

		if (m_pParent != NULL)
		{
			m_pParent->notifyChildrenChanged();
		}
	}
}