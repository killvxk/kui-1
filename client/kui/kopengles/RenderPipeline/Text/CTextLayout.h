#pragma once

namespace OpenHMI
{
	class CTextLayout
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum
		struct tagTextGlyph
		{
			INT				iLeft;
			INT				iTop;
			CCacheGlyphPtr	ptrCacheGlyph;
			const FT_Bitmap	*pBitmap;

			tagTextGlyph()
				: ptrCacheGlyph(NULL)
			{
				iLeft = 0;
				iTop = 0;
				pBitmap = NULL;
			}
		};

		struct tagTextRenderLine
		{
			INT					iWidth;
			INT					iHeight;
			List<tagTextGlyph>	listTextGlyph;

			tagTextRenderLine()
			{
				iWidth = 0;
				iHeight = 0;
			}
		};

	public:// method	
		CTextLayout();
		virtual ~CTextLayout();
		
		void	getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight);
		CDIB*	render(CRenderText *pRndText);
		UINT	getLineHeight();

	protected:// method	

	private:// method
		BOOLEAN	isCRLF(CHAR cChar) { return (cChar == L'\n'); };

		INT		calcWordWidth(const String &strContent);
		void	buildRenderLine();
		void	buildRenderLineLTR();
		void	buildRenderLineRTL();
		void	calcContentSize(INT &iWidth, INT &iHeight, INT iLineSpacing = 0);
		CDIB*	buildDibImage(INT iWidth, INT iHeight);
		void	paintLine(CDIB* pDibDest, tagTextRenderLine* pRenderLine, INT iLeft, INT iTop);
		void	parseToWordList(Array<String> &aryWordList);

	protected:// property

	private:// property
		CFontFace*					m_pFontFace;
		CRenderText*				m_pRndText;
		Array<tagTextRenderLine>	m_aryRenderLine;
	};
}