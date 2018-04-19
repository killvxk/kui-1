#include "OpenHMI.h"

namespace OpenHMI
{
	CRenderText::CRenderText()
		: m_eBiDiMode(BDM_LEFT_TO_RIGHT)
		, m_iLineSpacing(0)
		, m_iOffsetX(0)
		, m_iOffsetY(0)
		, m_bUseOffsetX(FALSE)
		, m_bUseOffsetY(FALSE)
		, m_eWrapLine(WL_NOWRAP)
		, m_eHorizontalLayout(HL_CENTER)
		, m_eVerticalLayout(VL_MIDDLE)
		, m_bHoritontalDuplicateSwitch(FALSE)
		, m_iHoritontalDuplicateInterval(0)
	{
		m_eType = CRenderItem::RIT_TEXT;
		m_font.setParentText(this);
	}

	CRenderText::~CRenderText()
	{
		;
	}

	void CRenderText::setContent(const String &strValue)
	{
		if ( strValue == m_strContent )
		{
			return;
		}

		m_strContent = strValue;

		notifyContentPropertyChanged();
	}

	void CRenderText::setBiDiMode(EBiDiMode eValue)
	{
		if ( eValue == m_eBiDiMode )
		{
			return;
		}

		m_eBiDiMode = eValue;

		notifyContentPropertyChanged();
	}

	void CRenderText::setLineSpacing(INT iValue)
	{
		if ( iValue == m_iLineSpacing )
		{
			return;
		}

		m_iLineSpacing = iValue;

		notifyContentPropertyChanged();
	}

	void CRenderText::setOffsetX(INT iValue)
	{ 
		if ( iValue == m_iOffsetX )
		{
			return;
		}

		m_iOffsetX = iValue; 
		
		notifyMergePropertyChanged();
	}

	void CRenderText::setOffsetY(INT iValue)
	{ 
		if ( iValue == m_iOffsetY )
		{
			return;
		}

		m_iOffsetY = iValue; 

		notifyMergePropertyChanged();
	}

	void CRenderText::setUseOffsetX(BOOLEAN bValue)
	{
		if ( bValue == m_bUseOffsetX )
		{
			return;
		}

		m_bUseOffsetX = bValue; 

		notifyMergePropertyChanged();
	}

	void CRenderText::setUseOffsetY(BOOLEAN bValue)	
	{ 
		if ( bValue == m_bUseOffsetY )
		{
			return;
		}

		m_bUseOffsetY = bValue; 

		notifyMergePropertyChanged();
	}

	void CRenderText::setWrapLine(EWrapLine eValue)
	{
		if ( eValue == m_eWrapLine )
		{
			return;
		}

		m_eWrapLine = eValue;

		notifyContentPropertyChanged();
	}

	void CRenderText::setHorizontalLayout(EHorizontalLayout eValue)
	{
		if ( eValue == m_eHorizontalLayout )
		{
			return;
		}

		m_eHorizontalLayout = eValue;

		notifyMergePropertyChanged();
	}

	void CRenderText::setVerticalLayout(EVerticalLayout eValue)
	{
		if ( eValue == m_eVerticalLayout )
		{
			return;
		}

		m_eVerticalLayout = eValue;

		notifyMergePropertyChanged();
	}

	void CRenderText::setHoritontalDuplicateSwitch(BOOLEAN bValue)
	{
		if ( bValue == m_bHoritontalDuplicateSwitch )
		{
			return;
		}

		m_bHoritontalDuplicateSwitch = bValue;

		notifyMergePropertyChanged();
	}

	void CRenderText::setHoritontalDuplicateInterval(INT iValue)
	{
		if ( iValue == m_iHoritontalDuplicateInterval )
		{
			return;
		}

		m_iHoritontalDuplicateInterval = iValue;

		notifyMergePropertyChanged();
	}
}