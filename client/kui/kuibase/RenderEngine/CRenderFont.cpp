#include "OpenHMI.h"

namespace OpenHMI
{
	CRenderFont::CRenderFont()
		: m_pText(NULL)
		, m_uiColor(0xFF000000)
		, m_uiSize(24)
		, m_eStyle(CRenderFont::FS_REGULAR)
	{
		;
	}

	CRenderFont::~CRenderFont()
	{
		;
	}

	void CRenderFont::setName(const String &strValue)
	{
		if( strValue == m_strName )
		{
			return;
		}

		m_strName = strValue;

		m_pText->notifyContentPropertyChanged();
	}

	void CRenderFont::setColor(UINT uiValue)
	{
		if( uiValue == m_uiColor )
		{
			return;
		}

		m_uiColor = uiValue;

		m_pText->notifyContentPropertyChanged();
	}

	void CRenderFont::setSize(UINT uiValue)
	{
		if( uiValue == m_uiSize )
		{
			return;
		}

		m_uiSize = uiValue;

		m_pText->notifyContentPropertyChanged();
	}

	void CRenderFont::setStyle(EFontStyle eValue)
	{
		if( eValue == m_eStyle )
		{
			return;
		}

		m_eStyle = eValue;

		m_pText->notifyContentPropertyChanged();
	}
}