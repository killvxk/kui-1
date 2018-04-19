
#include "RenderEngine.h"

#include "CTextLayout.h"

namespace OpenHMI
{

	CTextLayout::CTextLayout()
		: m_pFontFace(NULL)
		, m_pRndText(NULL)
	{
		;
	}

	CTextLayout::~CTextLayout()
	{
		;
	}	

	void CTextLayout::getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight)
	{
		if (puiWidth != NULL)
		{
			*puiWidth = 0;
		}

		if (puiHeight != NULL)
		{
			*puiHeight = 0;
		}

		if (puiLineHeight != NULL)
		{
			*puiLineHeight = 0;
		}

		m_pRndText = pRndText;		

		if (m_pRndText == NULL || m_pRndText->getContent().length() == 0)
		{
			return;
		}

		const CRenderFont *pRndFont = m_pRndText->getFont();
		if (pRndFont->getName().length() == 0 || pRndFont->getSize() == 0)
		{
			return;
		}

		m_pFontFace = CFontPool::getInstance()->getFontFace(pRndFont->getName(), pRndFont->getSize());
		if (m_pFontFace == NULL)
		{
			return;
		}

		buildRenderLine();

		INT iWidth = 0;
		INT iHeight = 0;
		calcContentSize(iWidth, iHeight, m_pRndText->getLineSpacing());

		if (puiWidth != NULL)
		{
			*puiWidth = iWidth;
		}

		if (puiHeight != NULL)
		{
			*puiHeight = iHeight;
		}

		if (puiLineHeight != NULL)
		{
			if (m_aryRenderLine.size() > 0)
			{
				*puiLineHeight = m_aryRenderLine[0]->iHeight;
			}
			else
			{
				*puiLineHeight = 0;
			}
		}
	}

	CDIB* CTextLayout::render(CRenderText *pRndText)
	{
		UINT uiWidth = 0;
		UINT uiHeight = 0;

		getTextInfo(pRndText, &uiWidth, &uiHeight, NULL);

		return buildDibImage(uiWidth, uiHeight);
	}

	UINT CTextLayout::getLineHeight()
	{
		if (m_aryRenderLine.size() > 0)
		{
			return m_aryRenderLine[0]->iHeight;
		}
		else
		{
			return 0;
		}
	}

	INT  CTextLayout::calcWordWidth(const String &strContent)
	{
		FT_UInt uiCurIndex = 0;
		FT_UInt uiPreIndex = 0;
		INT iPenX = 0;
		CCacheGlyph	*pCacheGlyph = NULL;

		/*
		if (strContent.length() == 1)
		{
			CHAR cTemp = ((const CHAR*)strContent)[0];
			if ((cTemp == L' ') || (cTemp == L',') || (cTemp == L'.') ||
				(cTemp == L':') || (cTemp == L'!') || (cTemp == L'?'))
			{
				return 0;
			}
		}
		*/

		INT iWidth = 0;
		for (UINT i = 0; i < strContent.length(); i++)
		{
			// Render glyph
			CHAR cCode = strContent[i];

			// if char is '\n'
			if (isCRLF(cCode))
			{
				continue;
			}

			uiCurIndex = m_pFontFace->getCharIndex(cCode);
			pCacheGlyph = m_pFontFace->getGlyph(cCode, UINT(m_pRndText->getFont()->getStyle()));
			if (pCacheGlyph == NULL)
			{
				continue;
			}

			// Do kerning
			iPenX += m_pFontFace->getKerning(uiPreIndex, uiCurIndex) >> 6;
			
			const FT_Glyph_Metrics* pGlyphMetrics = pCacheGlyph->getMetrics();
			const FT_Bitmap* pBitmap = pCacheGlyph->getBitmap();

			INT iBmpX = iPenX + (pGlyphMetrics->horiBearingX >> 6);
			iPenX += pGlyphMetrics->horiAdvance >> 6;

			if ((pBitmap != NULL) && ((iBmpX + (INT)pBitmap->width) > iPenX))
			{
				iWidth = iBmpX + pBitmap->width;
			}
			else
			{
				iWidth = iPenX;
			}

			uiPreIndex = uiCurIndex;
		}

		return iWidth;
	}

	void CTextLayout::buildRenderLine()
	{
		if (m_pRndText->getBiDiMode() == CRenderText::BDM_LEFT_TO_RIGHT)
		{
			buildRenderLineLTR();
		}
		else
		{
			buildRenderLineRTL();
		}
	}

	void CTextLayout::buildRenderLineLTR()
	{
		Array<String> aryWordList;

		// If NO wrap by word, we can regard the whole string as a word.
		if (m_pRndText->getWrapLine() == CRenderText::WL_WRAP_BY_WORD)
		{
			parseToWordList(aryWordList);
		}
		else
		{
			String * strWord = new String(m_pRndText->getContent());
			aryWordList.addItem(strWord);
		}

		m_aryRenderLine.clear();

		FT_UInt uiCurIndex = 0;
		FT_UInt uiPreIndex = 0;
		INT iPenX = 0;
		INT iPenY = m_pFontFace->getSizeMatrics()->ascender >> 6;
		CCacheGlyph	*pCacheGlyph = NULL;

		// Create the first line.
		tagTextRenderLine* pRenderLine = new tagTextRenderLine();
		m_aryRenderLine.addItem(pRenderLine);
		pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;

		aryWordList.resetIterator();
		while (aryWordList.hasMore())
		{
			const String & strContent = *(aryWordList.next());

			if (m_pRndText->getWrapLine() == CRenderText::WL_WRAP_BY_WORD)
			{
				// Do Wrap by Word: if it not enough width to show current word, we should create new line.
				INT iWordWidth = calcWordWidth(strContent);
				if ((iPenX > 0) && ((iPenX + iWordWidth) > INT(m_pRndText->getWidth())))				
				{
					pRenderLine = new tagTextRenderLine();
					m_aryRenderLine.addItem(pRenderLine);
					pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;
					iPenX = 0;
					uiPreIndex = 0;
				}
			}

			for (UINT i = 0; i < strContent.length(); i++)
			{
				// Render glyph
				CHAR cCode = strContent[i];

				// if char is '\n', we should create a new line.
				if (isCRLF(cCode))
				{
					pRenderLine = new tagTextRenderLine();
					m_aryRenderLine.addItem(pRenderLine);
					pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;
					iPenX = 0;
					uiPreIndex = 0;
					continue;
				}

				uiCurIndex = m_pFontFace->getCharIndex(cCode);
				pCacheGlyph = m_pFontFace->getGlyph(cCode, UINT(m_pRndText->getFont()->getStyle()));
				if (pCacheGlyph == NULL)
				{
					continue;
				}

				// Do kerning
				iPenX += m_pFontFace->getKerning(uiPreIndex, uiCurIndex) >> 6;
				
				const FT_Glyph_Metrics* pGlyphMetrics = pCacheGlyph->getMetrics();

				// Process text wrap
				if (m_pRndText->getWrapLine() == CRenderText::WL_WRAP_BY_CHAR)
				{
					// Do Wrap by Char: if it not enough width to show current char, we should create new line.
					if ((iPenX > 0) && ((iPenX + (pGlyphMetrics->horiBearingX >> 6) + (pGlyphMetrics->width >> 6)) > INT(m_pRndText->getWidth())))
					{
						pRenderLine = new tagTextRenderLine();
						m_aryRenderLine.addItem(pRenderLine);
						pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;
						iPenX = 0;
						uiPreIndex = 0;
					}
				}

				// Generate render glyph
				tagTextGlyph *pTextGlyph = new tagTextGlyph();

				pTextGlyph->iLeft = iPenX + (pGlyphMetrics->horiBearingX >> 6);
				pTextGlyph->iTop = iPenY - (pGlyphMetrics->horiBearingY >> 6);
				pTextGlyph->ptrCacheGlyph = pCacheGlyph;
				pTextGlyph->pBitmap = pCacheGlyph->getBitmap();
				pRenderLine->listTextGlyph.addLast(pTextGlyph);

				iPenX += pGlyphMetrics->horiAdvance >> 6;

				if ((pTextGlyph->pBitmap) && ((pTextGlyph->iLeft + (INT)pTextGlyph->pBitmap->width) > iPenX))
				{
					pRenderLine->iWidth = pTextGlyph->iLeft + pTextGlyph->pBitmap->width; 
				}
				else
				{
					pRenderLine->iWidth = iPenX;
				}
				uiPreIndex = uiCurIndex;
			}
		}
	}

	void CTextLayout::buildRenderLineRTL()
	{
		Array<String> aryWordList;

		// If NO wrap by word, we can regard the whole string as a word.
		if (m_pRndText->getWrapLine() == CRenderText::WL_WRAP_BY_WORD)
		{
			parseToWordList(aryWordList);
		}
		else
		{
			String * strWord = new String(m_pRndText->getContent());
			aryWordList.addItem(strWord);
		}

		m_aryRenderLine.clear();

		FT_UInt uiCurIndex = 0;
		FT_UInt uiPreIndex = 0;
		INT iPenX = 0;
		INT iPenY = m_pFontFace->getSizeMatrics()->ascender >> 6;
		CCacheGlyph	*pCacheGlyph = NULL;

		// Create the first line.
		tagTextRenderLine* pRenderLine = new tagTextRenderLine();
		m_aryRenderLine.addItem(pRenderLine);
		pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;

		aryWordList.resetIterator();
		while (aryWordList.hasMore())
		{
			const String & strContent = *(aryWordList.next());

			if (m_pRndText->getWrapLine() == CRenderText::WL_WRAP_BY_WORD)
			{
				// Do Wrap by Word: if it not enough width to show current word, we should create new line.
				INT iWordWidth = calcWordWidth(strContent);
				if ((iPenX < 0) && ((-iPenX + iWordWidth) > INT(m_pRndText->getWidth())))				
				{
					pRenderLine = new tagTextRenderLine();
					m_aryRenderLine.addItem(pRenderLine);
					pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;
					iPenX = 0;
					uiPreIndex = 0;
				}
			}

			for (UINT i = 0; i < strContent.length(); i++)
			{
				// Render glyph
				CHAR cCode = strContent[i];

				// if char is '\n', we should create a new line.
				if (isCRLF(cCode))
				{
					pRenderLine = new tagTextRenderLine();
					m_aryRenderLine.addItem(pRenderLine);
					pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;
					iPenX = 0;
					uiPreIndex = 0;
					continue;
				}

				uiCurIndex = m_pFontFace->getCharIndex(cCode);
				pCacheGlyph = m_pFontFace->getGlyph(cCode, UINT(m_pRndText->getFont()->getStyle()));
				if (pCacheGlyph == NULL)
				{
					continue;
				}

				// Do kerning
				iPenX -= m_pFontFace->getKerning(uiCurIndex, uiPreIndex) >> 6;
				
				const FT_Glyph_Metrics* pGlyphMetrics = pCacheGlyph->getMetrics();

				// Process text wrap
				if (m_pRndText->getWrapLine() == CRenderText::WL_WRAP_BY_CHAR)
				{
					// Do Wrap by Char: if it not enough width to show current char, we should create new line.
					if ((iPenX < 0) && ((iPenX - (pGlyphMetrics->horiAdvance >> 6) + (pGlyphMetrics->horiBearingX >> 6)) < -INT(m_pRndText->getWidth())))
					{
						pRenderLine = new tagTextRenderLine();
						m_aryRenderLine.addItem(pRenderLine);
						pRenderLine->iHeight = m_pFontFace->getSizeMatrics()->height >> 6;
						iPenX = 0;
						uiPreIndex = 0;
					}
				}

				// Generate render glyph
				tagTextGlyph *pTextGlyph = new tagTextGlyph();

				iPenX -= pGlyphMetrics->horiAdvance >> 6;
				pTextGlyph->iLeft = iPenX + (pGlyphMetrics->horiBearingX >> 6);
				pTextGlyph->iTop = iPenY - (pGlyphMetrics->horiBearingY >> 6);
				pTextGlyph->ptrCacheGlyph = pCacheGlyph;
				pTextGlyph->pBitmap = pCacheGlyph->getBitmap();
				pRenderLine->listTextGlyph.addLast(pTextGlyph);

				pRenderLine->iWidth = iPenX;
				uiPreIndex = uiCurIndex;
			}
		}

		m_aryRenderLine.resetIterator();
		while (m_aryRenderLine.hasMore())
		{
			pRenderLine = m_aryRenderLine.next();
			if (pRenderLine->iWidth >= 0)
			{
				continue;
			}
			pRenderLine->iWidth = -pRenderLine->iWidth;
			pRenderLine->listTextGlyph.resetIterator();
			while (pRenderLine->listTextGlyph.hasMore())
			{
				tagTextGlyph *pTextGlyph = pRenderLine->listTextGlyph.next();
				pTextGlyph->iLeft = pRenderLine->iWidth + pTextGlyph->iLeft;
			}
		}
	}

	void CTextLayout::calcContentSize(INT &iWidth, INT &iHeight, INT iLineSpacing)
	{
		iWidth = 0;
		iHeight = 0;

		m_aryRenderLine.resetIterator();
		while (m_aryRenderLine.hasMore())
		{
			tagTextRenderLine* pRenderLine = m_aryRenderLine.next();
			iWidth = Math::getMax(iWidth, pRenderLine->iWidth);
			iHeight += pRenderLine->iHeight;
		}

		// Add line spacing
// 		if ((iLineSpacing != 0) && (m_aryRenderLine.size() >= 2))
// 		{
// 			iHeight += iLineSpacing * (m_aryRenderLine.size() - 1);
// 		}
		iHeight += iLineSpacing * m_aryRenderLine.size();
	}

	CDIB* CTextLayout::buildDibImage(INT iWidth, INT iHeight)
	{
		if (iWidth == 0 || iHeight == 0)
		{
			return NULL;
		}

		CDIB* pDibImage = new CDIB();
		pDibImage->createDIB(iWidth, iHeight);

		INT iLineTop = 0;
		m_aryRenderLine.resetIterator();
		while (m_aryRenderLine.hasMore())
		{
			tagTextRenderLine* pRenderLine = m_aryRenderLine.next();

			INT iLineLeft = 0;
			switch (m_pRndText->getHorizontalLayout())
			{
			case CRenderText::HL_LEFT:
				break;

			case CRenderText::HL_CENTER:
				iLineLeft = (iWidth - pRenderLine->iWidth) / 2; 
				break;

			case CRenderText::HL_RIGHT:
				iLineLeft = iWidth - pRenderLine->iWidth;
				break;
			}

			paintLine(pDibImage, pRenderLine, iLineLeft, iLineTop);

			iLineTop += pRenderLine->iHeight + m_pRndText->getLineSpacing();
		}

		return pDibImage;
	}

	void CTextLayout::paintLine(CDIB* pDibDest, tagTextRenderLine* pRenderLine, INT iLeft, INT iTop)
	{
		if (pRenderLine->iWidth == 0)
		{
			return;
		}

		// Paint glyph to DIB image.
		pRenderLine->listTextGlyph.resetIterator();
		while (pRenderLine->listTextGlyph.hasMore())
		{
			tagTextGlyph *pTextGlyph = pRenderLine->listTextGlyph.next();
			pDibDest->merge8DIB(	iLeft + pTextGlyph->iLeft, iTop + pTextGlyph->iTop,
									pTextGlyph->pBitmap->buffer, pTextGlyph->pBitmap->width, pTextGlyph->pBitmap->rows,
									255, m_pRndText->getFont()->getColor());
		}
	}

	void CTextLayout::parseToWordList(Array<String> &aryWordList)
	{
		#define IS_NUMBER(c)	((c >= 0x30) && (c <= 0x39))
		#define IS_LETTER(c)	(((c >= 0x41) && (c <= 0x5A)) || ((c >= 0x61) && (c <= 0x7A)))
		#define IS_CHAR(var, c)	(var == c)
		#define BUFSIZE 1024

		const String &strContent = m_pRndText->getContent();

		aryWordList.clear();
		if (strContent.length() == 0)
		{
			return;
		}

		CHAR		cTemp;
		CHAR		cWord[BUFSIZE] = {0};
		UINT		nIndex = 0;
		String*		pStrWord = NULL;
		const CHAR*	pcContent = (const CHAR*)(strContent);
		const CHAR*	pcEnd = pcContent + strContent.length();

		while (pcContent < pcEnd)
		{
			cTemp = *pcContent;

			// this is a english word
			if (IS_LETTER(cTemp) || IS_NUMBER(cTemp) || IS_CHAR(cTemp, L'-'))
			{
				cWord[nIndex++] = cTemp;
				pcContent++;

				while (pcContent < pcEnd && nIndex < (BUFSIZE - 1))
				{
					cTemp = *pcContent;
					if (IS_LETTER(cTemp) || IS_NUMBER(cTemp) || IS_CHAR(cTemp, L'-'))
					{
						cWord[nIndex++] = cTemp;
						pcContent++;
					}
					else
					{
						break;
					}
				}

				cWord[nIndex] = L'\0';
				nIndex = 0;
				pStrWord = new String(cWord);
				aryWordList.addItem(pStrWord);
				continue;
			}		

			// this is a \r		
			if (IS_CHAR(cTemp, L'\r'))
			{
				cWord[nIndex] = L'\n';
				pcContent++;

				// Ignore \n after \r
				if (IS_CHAR(cTemp, L'\n'))
				{
					pcContent++;
				}

				cWord[nIndex] = '\0';
				nIndex = 0;
				pStrWord = new String(cWord);
				aryWordList.addItem(pStrWord);
	
				continue;
			}

			// this is a \n
			if (IS_CHAR(cTemp, L'\n'))
			{
				cWord[nIndex++] = L'\n';
				pcContent++;

				cWord[nIndex] = '\0';
				nIndex = 0;
				pStrWord = new String(cWord);
				aryWordList.addItem(pStrWord);

				continue;
			}

			// this is a TAB, we expand it to 4 blackspace.
			if (IS_CHAR(cTemp, L'\t'))
			{
				for (INT i = 0; i < 4; ++i)
				{
					cWord[nIndex] = L' ';
					nIndex++;
				}
				pcContent++;

				cWord[nIndex] = '\0';
				nIndex = 0;
				pStrWord = new String(cWord);
				aryWordList.addItem(pStrWord);
				continue;
			}

			// Other charactors.
			cWord[nIndex++] = cTemp;
			pcContent++;
			cWord[nIndex] = '\0';
			nIndex = 0;			
			pStrWord = new String(cWord);
			aryWordList.addItem(pStrWord);
		}		
	}
}