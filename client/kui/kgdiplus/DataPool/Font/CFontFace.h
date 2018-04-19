#pragma once

namespace OpenHMI
{
	class CFontFace : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagCacheGlyphKey
		{
			FT_ULong	ulCode;
			UINT		uiStyle;

			INT operator()(const tagCacheGlyphKey &key1, const tagCacheGlyphKey &key2) const
			{
				INT iRet = key1.ulCode  - key2.ulCode;
				
				if (iRet < 0)
				{
					return -1;
				}
				else if (iRet == 0)
				{
					if (key1.uiStyle == key2.uiStyle)
					{
						return 0;
					}
					else if (key1.uiStyle < key2.uiStyle)
					{
						return -1;
					}
					else
					{
						return 1;
					}
				}
				else
				{
					return 1;
				}
			}
		};

	public:// method 
		CFontFace(const FT_Library ftLib, const String &strFontFile, UINT uiIndex, UINT uiFontSize);
		virtual ~CFontFace(void);

		FT_Face					getHandle()						{ return m_face; }
		INT						getCharIndex(FT_ULong ulCode)	{ return FT_Get_Char_Index(m_face, ulCode); } 
		const FT_Size_Metrics*	getSizeMatrics()				{ return &(m_face->size->metrics); }

		INT						getKerning(UINT uiIndexL, UINT uiIndexR);
		CCacheGlyph*			getGlyph(FT_ULong ulCode, UINT uiStyle);		

	protected:// method
		CFontFace(const CFontFace &){}
		CFontFace &operator = (const CFontFace &){return *this;}	

	private:// method	

	protected:// property

	private:
		FT_Face			m_face;

		CachePool<tagCacheGlyphKey, CCacheGlyph,tagCacheGlyphKey>	m_cachePool;		
	};
}