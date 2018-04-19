#include "OpenHMI.h"

namespace OpenHMI
{
	CRenderGroup::CRenderGroup(BOOLEAN bManageLifeCycle)
		: m_aryChildren(bManageLifeCycle)
		, m_bNeedCache(TRUE)		
		, m_bNeedClip(FALSE)	
		, m_bHasViewport(FALSE)
		, m_bFrequentChangeFlag(FALSE)
		, m_bCanMakeCache(TRUE)
		, m_uiAlpha(255)
		, m_fRotateRadian(0.0f)
		, m_eRA(RA_Z)
		, m_iAnchorX(0)
		, m_iAnchorY(0)
		, m_fScaleWidth(1.0f)
		, m_fScaleHeight(1.0f)
		, m_aryFilter(bManageLifeCycle)
	{
		m_eType = CRenderItem::RIT_GROUP;
	}

	CRenderGroup::~CRenderGroup()
	{
		;
	}

	BOOLEAN CRenderGroup::isRenderTextWrapper()
	{
		if (m_aryChildren.size() != 1)
		{
			return FALSE;
		}

		if (m_aryChildren[0]->getType() == CRenderItem::RIT_TEXT)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CRenderGroup::isRenderImageWrapper()
	{
		if (m_aryChildren.size() != 1)
		{
			return FALSE;
		}

		if (m_aryChildren[0]->getType() == CRenderItem::RIT_IMAGE)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}	

	BOOLEAN CRenderGroup::isRenderViewportWrapper()
	{
		if (m_aryChildren.size() != 1)
		{
			return FALSE;
		}

		if (m_aryChildren[0]->getType() == CRenderItem::RIT_VIEWPORT)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void CRenderGroup::setAlpha(UINT uiValue)
	{
		if( m_uiAlpha == uiValue )
		{
			return;
		}

		m_uiAlpha = uiValue;

		notifyMergePropertyChanged();
	}

	void CRenderGroup::setRotateRadian(FLOAT fValue, INT iAnchorX, INT iAnchorY, ERotateAxis eRA)
	{
		if( eRA == m_eRA && Math::isZero(fValue - m_fRotateRadian) && iAnchorX == m_iAnchorX && iAnchorY == m_iAnchorY )
		{
			return ;
		}

		m_fRotateRadian = fValue;
		m_iAnchorX = iAnchorX;
		m_iAnchorY = iAnchorY;
		m_eRA			= eRA;

		notifyMergePropertyChanged();
	}

	void CRenderGroup::setScaleWidth(FLOAT fValue)
	{
		if( fValue < 0 || Math::isZero(fValue - m_fScaleWidth))
		{
			return;
		}

		m_fScaleWidth = fValue;

		notifyMergePropertyChanged();
	}

	void CRenderGroup::setScaleHeight(FLOAT fValue)
	{
		if( fValue < 0 || Math::isZero(fValue - m_fScaleHeight) )
		{
			return;
		}

		m_fScaleHeight = fValue;

		notifyMergePropertyChanged();
	}

	void CRenderGroup::clearAllFilter()
	{
		m_aryFilter.clear();

		notifyContentPropertyChanged();
	}

	void CRenderGroup::addFilter(CRenderBaseFilter *pFilter)
	{
		m_aryFilter.addItem(pFilter);

		notifyContentPropertyChanged();
	}

	void CRenderGroup::updateFilter()
	{
		notifyContentPropertyChanged();
	}

	void CRenderGroup::addRenderItem(CRenderItem *pRenderItem)
	{
		pRenderItem->setParent(this);

		m_aryChildren.deleteItem(pRenderItem);
		m_aryChildren.addItem(pRenderItem);
	}

	void CRenderGroup::insertRenderItem(UINT uiIndex, CRenderItem *pRenderItem)
	{
		pRenderItem->setParent(this);

		//INT	iItemIndex = (INT)m_aryChildren.getItemIndex(pRenderItem);
		//if( iItemIndex < uiIndex && iItemIndex >=0 )
		//{
		//	uiIndex--;
		//	m_aryChildren.deleteItem(pRenderItem);
		//}
		//else if( iItemIndex == uiIndex )
		//{
		//	return;
		//}
		//else if( iItemIndex == -1 )
		//{
		//	;
		//}
		//else
		//{
		//	m_aryChildren.deleteItem(pRenderItem);
		//}
		//
		//m_aryChildren.insertItem(uiIndex, pRenderItem);

		m_aryChildren.insertItem(uiIndex, pRenderItem);
	}

	void CRenderGroup::destroy()
	{
		m_aryBackupChildren.clear();

		CRenderItem *pItem = NULL;

		for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
		{
			pItem = m_aryChildren[ui];

			if ( pItem->getType() == CRenderItem::RIT_GROUP )
			{
				static_cast<CRenderGroup *>(pItem)->destroy();
			}
		}

		m_aryChildren.clear();

		m_aryFilter.clear();

		m_bCanMakeCache = TRUE;
		m_bNeedCache = TRUE;
		m_bHasViewport = FALSE;
	}

	void CRenderGroup::breakUp()
	{
// 		CRenderItem *pItem = NULL;
// 
// 		for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
// 		{
// 			pItem = m_aryChildren[ui];
// 
// 			if ( pItem->getType() == CRenderItem::RIT_GROUP )
// 			{
// 				static_cast<CRenderGroup *>(pItem)->breakUp();
// 			}
// 			else
// 			{
// 				pItem->setParent(NULL);
// 			}
// 		}
// 
// 		m_aryChildren.clear();
// 
// 		setParent(NULL);

		m_aryBackupChildren.clear();

		CRenderItem *pItem = NULL;

		for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
		{
			pItem = m_aryChildren[ui];

			if ( pItem->getType() == CRenderItem::RIT_GROUP )
			{
				static_cast<CRenderGroup *>(pItem)->breakUp();
			}

			m_aryBackupChildren.addItem(pItem);
		}

		m_aryChildren.clear();

		m_bCanMakeCache = TRUE;
		m_bNeedCache = TRUE;
		m_bHasViewport = FALSE;
	}

	void CRenderGroup::checkChildrenChanged()
	{
		BOOLEAN bChildrenChanged = FALSE;

		if (m_aryChildren.size() != m_aryBackupChildren.size())
		{
			bChildrenChanged = TRUE;
		}

		CRenderItem *pItem = NULL;

		for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
		{
			pItem = m_aryChildren[ui];

			if ( pItem->getType() == CRenderItem::RIT_GROUP )
			{
				static_cast<CRenderGroup *>(pItem)->checkChildrenChanged();
			}			

			if (ui < m_aryBackupChildren.size() && pItem != m_aryBackupChildren[ui])
			{
				bChildrenChanged = TRUE;
			}
		}

		if (bChildrenChanged)
		{
			notifyChildrenChanged();
		}
	}

	void CRenderGroup::updateCacheFlagStep1()
	{
		if (m_bFrequentChangeFlag)
		{
			m_bNeedCache = FALSE;

			CRenderGroup *pParent = getParent();
			while (pParent != NULL && pParent->getNeedCache())
			{
				pParent->setNeedCache(FALSE);
				pParent = pParent->getParent();
			}
		}

		BOOLEAN bHasDynamicFilter = FALSE;
		for( UINT i = 0; i < m_aryFilter.size(); ++i)
		{
			CRenderBaseFilter* pFilter = m_aryFilter[i];
			if( pFilter->isDynamic() && pFilter->isPlaying() )
			{
				bHasDynamicFilter = TRUE;
				break;
			}
		}

		if( bHasDynamicFilter )
		{
			CRenderGroup *pParent = getParent();
			while (pParent != NULL)
			{
				pParent->setCanMakeCache(FALSE);
				pParent = pParent->getParent();
			}
		}

		if (getWidth() == 0 || getHeight() == 0)
		{
			m_bNeedCache = FALSE;
		}

		CRenderItem *pItem = NULL;
		BOOLEAN bLeaf = TRUE;

		for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
		{
			pItem = m_aryChildren[ui];

			if (pItem->getType() == CRenderItem::RIT_GROUP)
			{
				static_cast<CRenderGroup *>(pItem)->updateCacheFlagStep1();
				bLeaf = FALSE;
			}
			else if (pItem->getType() == CRenderItem::RIT_VIEWPORT)
			{
				CRenderGroup *pParent = getParent();
				if (pParent != NULL)
				{
					pParent->setHasViewport(TRUE);
				}
			}
		}

		if ( bLeaf )
		{
			m_bNeedCache = FALSE;
		}
	}

	void CRenderGroup::updateCacheFlagStep2()
	{
		if (m_bNeedCache)
		{
			CRenderItem *pItem = NULL;
			CRenderGroup *pGroup = NULL;

			for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
			{
				pItem = m_aryChildren[ui];

				if ( pItem->getType() == CRenderItem::RIT_GROUP )
				{
					pGroup = static_cast<CRenderGroup *>(pItem);

					pGroup->setNeedCache(FALSE);
					pGroup->updateCacheFlagStep2();
				}
			}
		}
		else
		{
			CRenderItem *pItem = NULL;
			CRenderGroup *pGroup = NULL;

			for ( UINT ui = 0; ui < m_aryChildren.size(); ui++ )
			{
				pItem = m_aryChildren[ui];

				if ( pItem->getType() == CRenderItem::RIT_GROUP )
				{
					pGroup = static_cast<CRenderGroup *>(pItem);
					pGroup->updateCacheFlagStep2();
				}
			}
		}
	}

	CRenderItem* CRenderGroup::hitTest(INT iX, INT iY)
	{
		CRenderItem* pHitItem = NULL;

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			pHitItem = m_aryChildren[i]->hitTest(iX, iY);
			if (pHitItem != NULL)
			{
				return pHitItem;
			}
		}

		return CRenderItem::hitTest(iX, iY);
	}
}