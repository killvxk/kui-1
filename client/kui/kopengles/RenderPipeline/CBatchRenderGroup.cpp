#include "RenderEngine.h"

namespace OpenHMI
{
	CBatchRenderGroup::CBatchRenderGroup()
		: m_iRef(0)
		, m_iX(0)
		, m_iY(0)
		, m_uiWidth(0)
		, m_uiHeight(0)		
		, m_pParent(NULL)
		, m_pOwner(NULL)
		, m_pRT(NULL)
		, m_bPosChanged(TRUE)
		, m_bSizeChanged(TRUE)
	{
		;
	}

	CBatchRenderGroup::~CBatchRenderGroup()
	{
		reset();
	}

	CRenderTarget* CBatchRenderGroup::getRenderTarget()
	{
		return m_pRT;
	}

	void CBatchRenderGroup::render()
	{
		;
	}

	void CBatchRenderGroup::flush(BOOLEAN bGenerateRT)
	{
		if ( bGenerateRT)
		{
			if ( m_pRT.isNull() )
			{
				m_pRT = CRenderTargetManager::getInstance()->getRenderTarget(m_uiWidth, m_uiHeight);
			}
			else 
			{
				if( m_pRT->getWidth() != m_uiWidth || m_pRT->getHeight() != m_uiHeight )
				{
					m_pRT->resize(m_uiWidth, m_uiHeight);
				}
			}
		}

		m_bPosChanged = FALSE;
		m_bSizeChanged = FALSE;
	} 

	void CBatchRenderGroup::clear()	
	{ 
		; 
	}

	void CBatchRenderGroup::reset()
	{
		if( m_pRT )
		{
			m_pRT->setInUse(FALSE);
			m_pRT = NULL;
		}

		m_uiWidth = 0;
		m_uiHeight = 0;
		m_pParent = NULL;
		m_iX = 0;
		m_iY = 0;
		m_bPosChanged = TRUE;
		m_bSizeChanged = TRUE;
	}

	void CBatchRenderGroup::set(INT iX, INT iY, UINT uiW, UINT uiH)			
	{ 
		if( m_iX != iX || m_iY != iY )
		{
			m_iX = iX; 
			m_iY = iY; 
			notifyPositionChanged();
		}

		if( m_uiWidth != uiW || m_uiHeight != uiH )
		{
			m_uiWidth = uiW; 
			m_uiHeight = uiH;	
			notifySizeChanged();
		}
	}

	void CBatchRenderGroup::setX(INT iValue)								
	{ 
		if( m_iX != iValue )
		{
			m_iX = iValue; 
			notifyPositionChanged();
		}		
	}
	
	void CBatchRenderGroup::setY(INT iValue)								
	{ 
		if( m_iY != iValue )
		{
			m_iY = iValue; 
			notifyPositionChanged();
		}		
	}
	
	void CBatchRenderGroup::setWidth(UINT uiValue)	
	{
		if( m_uiWidth != uiValue )
		{
			m_uiWidth = uiValue; 
			notifySizeChanged();
		}	
	}
	
	void CBatchRenderGroup::setHeight(UINT uiValue)	
	{
		if( m_uiHeight != uiValue )
		{
			m_uiHeight = uiValue;	
			notifySizeChanged();
		}
	}

	void CBatchRenderGroup::addRef()
	{
		++m_iRef;
	}

	void CBatchRenderGroup::release()
	{
		if (m_iRef > 0)
		{
			--m_iRef;
			if (m_iRef == 0)
			{
				delete this;
			}
		}
	}
}