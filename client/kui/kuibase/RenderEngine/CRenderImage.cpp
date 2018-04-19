#include "OpenHMI.h"
#pragma comment(lib,"gdiplus.lib")

namespace OpenHMI
{
	CRenderImage::CRenderImage()
		: m_eLayout(L_NORMAL)
		, m_iClipOffsetX(0)
		, m_iClipOffsetY(0)
		, m_bIsBackGround(FALSE)
		, m_pBitmap(NULL)
		, m_bIsBitmap(FALSE)
	{
		m_eType = CRenderItem::RIT_IMAGE;
	}

	CRenderImage::~CRenderImage()
	{
	
	}

	void CRenderImage::setFilePath(const String &strValue)
	{
		if( strValue == m_strFilePath )
		{
			return;
		}

		m_bIsBitmap = FALSE;
		m_strFilePath = strValue;

		notifyContentPropertyChanged();
	}

	const String& CRenderImage::getFilePath() const
	{
		return m_strFilePath;
	}

	void CRenderImage::setBitmap(const String& strID, Gdiplus::Bitmap* pBitmap)
	{
		if(pBitmap == NULL)
		{
			return;
		}

		if ((const CHAR*)strID == NULL)
			return;

		m_strFilePath = String::format(L"%s_w%d_h%d", (const CHAR*)strID, pBitmap->GetWidth(), pBitmap->GetHeight());
		m_pBitmap = pBitmap;
		m_bIsBitmap = TRUE;
		notifyContentPropertyChanged();
	}

	void CRenderImage::setLayout(ELayout eValue)
	{
		if( eValue == m_eLayout )
		{
			return;
		}

		m_eLayout = eValue;

		notifyContentPropertyChanged();
	}

	void CRenderImage::setNormalLayoutClipOffsetX(INT iValue)
	{
		if( m_iClipOffsetX == iValue )
		{
			return;
		}

		m_iClipOffsetX = iValue;

		notifyContentPropertyChanged();
	}

	void CRenderImage::setNormalLayoutClipOffsetY(INT iValue)
	{
		if( m_iClipOffsetY == iValue )
		{
			return;
		}

		m_iClipOffsetY = iValue;

		notifyContentPropertyChanged();
	}
}