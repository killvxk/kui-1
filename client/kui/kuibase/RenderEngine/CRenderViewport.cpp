#include "OpenHMI.h"

namespace OpenHMI
{
	CRenderViewport::CRenderViewport(BOOLEAN bManageLifeCycle)
		: m_bManageLifeCycle(bManageLifeCycle)
		, m_pStyle(NULL)
	{
		m_eType = CRenderItem::RIT_VIEWPORT;
	}

	CRenderViewport::~CRenderViewport()
	{
		if (m_bManageLifeCycle)
		{
			if (m_pStyle != NULL)
			{
				delete m_pStyle;
				m_pStyle = NULL;
			}
		}
	}

	void CRenderViewport::setStyle(CBaseStyle *pStyle)
	{
		if (m_pStyle == pStyle)
		{
			return;
		}

		if (m_bManageLifeCycle)
		{
			if (m_pStyle != NULL)
			{
				delete m_pStyle;
				m_pStyle = NULL;
			}
		}

		m_pStyle = pStyle;

		notifyContentPropertyChanged();
	}

	void CRenderViewport::updateStyle()
	{
		notifyContentPropertyChanged();
	}

	CRenderViewport::CBaseStyle::CBaseStyle()
		: m_eType (ST_NORMAL)
	{
		;
	}

	CRenderViewport::CStyleNormal::CStyleNormal()
	{
		m_eType = ST_NORMAL;
	}

	CRenderViewport::CStyleTransparent::CStyleTransparent()
	{
		m_eType = ST_TRANSPARENT;
	}

	CRenderViewport::CStyleCapture::CStyleCapture()
	{
		m_eType = ST_CAPTURE;
	}

	CRenderViewport::CStyleWaver::CStyleWaver()
		: uiWidth(300)
		, uiHeight(300)
		, uiRow(80)
		, uiColumn(30)
	{
		m_eType = ST_WAVER;
	}
}