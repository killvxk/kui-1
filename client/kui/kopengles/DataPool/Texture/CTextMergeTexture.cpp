#include "RenderEngine.h"

namespace OpenHMI
{
	CTextMergeTexture::CTextMergeTexture(UINT uiCellWidth, UINT uiCellHeight)
		: m_uiCellWidth(uiCellWidth)
		, m_uiCellHeight(uiCellHeight)
		, m_uiCellRow(0)
		, m_uiCellCol(0)
	{
		m_uiUsage = CCacheTexture::TEXUSAGE_TEXTMERGE;
	}

	CTextMergeTexture::~CTextMergeTexture()
	{
		;
	}

	void CTextMergeTexture::fill(UINT uidx, CDIB* pDIB)
	{
		if ( pDIB && isValid(uidx) )
		{
			dirty(uidx);
			INT iOffsetX = 0;
			INT iOffsetY = 0;
			getOffset(uidx,iOffsetX,iOffsetY);
			glBindTexture(GL_TEXTURE_2D, m_uiTexId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, iOffsetX, iOffsetY, pDIB->getWidth(), pDIB->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, (BYTE*)pDIB->getPixelData() );	
		}
	}

	void CTextMergeTexture::fill(const Array2<UINT>& aryIdx, CDIB* pDIB)
	{
		if ( pDIB && !aryIdx.isEmpty() )
		{
			dirty(aryIdx);
			INT iOffsetX = 0;
			INT iOffsetY = 0;
			getOffset(aryIdx[0],iOffsetX,iOffsetY);
			glBindTexture(GL_TEXTURE_2D, m_uiTexId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, iOffsetX, iOffsetY, pDIB->getWidth(), pDIB->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, (BYTE*)pDIB->getPixelData() );	
		}
	}

	void CTextMergeTexture::free(UINT uidx)
	{
		//CLog::info(String(L""),String::format(L"free %u",uidx));
		m_aryRectFreeFlag[uidx]	= TRUE;
	}

	void CTextMergeTexture::free(const Array2<UINT>& aryIdx)
	{
		UINT cnt = aryIdx.size();
		for( UINT i = 0 ; i < cnt; ++i )
		{
			m_aryRectFreeFlag[aryIdx[i]] = TRUE;	
		}
	}

	UINT CTextMergeTexture::getFreeRect() const
	{
		UINT cnt = m_aryRectFreeFlag.size();
		for ( UINT i = 0 ; i < cnt; ++i )
		{
			if( m_aryRectFreeFlag[i] )
			{
				return i;
			}
		}
		return -1;
	}

	BOOLEAN CTextMergeTexture::getFreeRect(UINT uiWidth, UINT uiHeight, Array2<UINT>& aryIdx) const
	{
		UINT col = uiHeight/m_uiCellHeight;
		if( (col * m_uiCellHeight) < uiHeight )
		{
			++col;
		}

		UINT row = uiWidth/m_uiCellWidth;
		if( (row * m_uiCellWidth) < uiWidth )
		{
			++row;
		}

		for( UINT i = 0 ; i < m_uiCellCol; ++i )
		{
			for(UINT j = 0 ; j < m_uiCellRow; ++j )
			{
				if( isFree(i,j,col,row) )
				{
					for( UINT m = 0 ; m < col; ++m )
					{
						for(UINT k = 0 ; k < row; ++k )
						{
							aryIdx.addItem((i+m)*m_uiCellRow + j + k);
						}
					}
					return TRUE;
				}
			}
		}

		return FALSE;
	}

	CRectangleF	CTextMergeTexture::getROI(UINT uidx, UINT uiWidth, UINT uiHeight) const
	{
		INT iOffsetX = 0;
		INT iOffsetY = 0;
		getOffset(uidx,iOffsetX,iOffsetY);

		CRectangleF rc(	FLOAT(iOffsetX)/m_uiWidth,			FLOAT(iOffsetY)/m_uiHeight, 
						FLOAT(uiWidth)/m_uiWidth,			FLOAT(uiHeight)/m_uiHeight);
		return rc;
	}

	void CTextMergeTexture::onGenerateGPUResource()
	{
		m_uiCellRow = m_uiWidth / m_uiCellWidth;
		m_uiCellCol = m_uiHeight / m_uiCellHeight;

		m_aryRectFreeFlag.addItems(m_uiCellRow * m_uiCellCol, TRUE);
	}

	void CTextMergeTexture::dirty(UINT uidx)
	{
		//CLog::info(String(L""),String::format(L"dirty %u",uidx));
		m_aryRectFreeFlag[uidx]	= FALSE;
	}

	void CTextMergeTexture::dirty(const Array2<UINT>& aryIdx)
	{
		UINT cnt = aryIdx.size();
		for( UINT i = 0 ; i < cnt; ++i )
		{
			m_aryRectFreeFlag[aryIdx[i]] = FALSE;	
		}
	}

	BOOLEAN	 CTextMergeTexture::isValid(UINT uidx) const
	{
		if( uidx < (m_uiCellRow * m_uiCellCol) )
		{
			return TRUE;
		}
		 
		return FALSE;
	}

	void CTextMergeTexture::getOffset(UINT uidx, INT& iOffsetX, INT& iOffsetY) const
	{
		UINT uiCellY = uidx/m_uiCellRow;
		UINT uiCellX = uidx - (uiCellY*m_uiCellRow);

		iOffsetX = INT( uiCellX * m_uiCellWidth);
		iOffsetY = INT( uiCellY * m_uiCellHeight);
	}

	BOOLEAN CTextMergeTexture::isFree(UINT uiStartCol, UINT uiStartRow, UINT uiColNum, UINT uiRowNum) const
	{
		if( (uiStartCol + uiColNum ) >= m_uiCellCol || (uiStartRow + uiRowNum) >= m_uiCellRow )
		{
			return FALSE;
		}

		for( UINT i = 0 ; i < uiColNum; ++i )
		{
			for(UINT j = 0 ; j < uiRowNum; ++j )
			{
				UINT idx = (uiStartCol+i) * m_uiCellRow + uiStartRow + j;
				if( !m_aryRectFreeFlag[idx] )
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
}