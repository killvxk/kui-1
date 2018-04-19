#include "RenderEngine.h"

namespace OpenHMI 
{
	CFontFace::CFontFace(const FT_Library ftLib, const String &strFontFile, UINT uiIndex, UINT uiFontSize) 
		: m_face(NULL)
	{
		m_cachePool.setPoolSize(CONFIG::getCharacterCacheCount());

		#define PATH_BUFF_LEN 512
		char acPath[PATH_BUFF_LEN] = {0};

		String::convertToMBCS(strFontFile, acPath, PATH_BUFF_LEN - 1);
		if (FT_New_Face(ftLib, acPath, uiIndex, &m_face))
		{
			return;
		}
		
		if (FT_Set_Char_Size(m_face, 0, uiFontSize * 64, 0, 0))
		{
			return;
		}
	}

	CFontFace::~CFontFace(void)
	{
		if (m_face != NULL)
		{
			FT_Done_Face(m_face);
			m_face = NULL;
		}
	}

	INT CFontFace::getKerning(UINT uiIndexL, UINT uiIndexR)
	{
        if (FT_HAS_KERNING(m_face) && (uiIndexL != 0) && (uiIndexR != 0))
        {
            FT_Vector kerning;
            FT_Get_Kerning(m_face, uiIndexL, uiIndexR, FT_KERNING_DEFAULT, &kerning);
            return kerning.x;
        }

		return 0;
	}

	CCacheGlyph* CFontFace::getGlyph(FT_ULong ulCode, UINT uiStyle)
	{
		tagCacheGlyphKey cacheGlyphKey;
		cacheGlyphKey.ulCode = ulCode;
		cacheGlyphKey.uiStyle = uiStyle;
		
		CCacheGlyph *pCacheGlyph = m_cachePool.getCacheObject(cacheGlyphKey);
		if (pCacheGlyph != NULL)
		{
			return pCacheGlyph;
		}

		// Render glyph to bitmap
		FT_UInt uiIndex = FT_Get_Char_Index(m_face, ulCode);
		if (FT_Load_Glyph(m_face, uiIndex, FT_LOAD_DEFAULT))
		{
			return NULL;
		}	
		
		if ((uiStyle & CRenderFont::FS_ITALIC) == CRenderFont::FS_ITALIC)
		{
			FT_Matrix matrix;
			matrix.xx = 0x10000L;
			matrix.xy = FT_Fixed(0.3 * 0x10000L);
			matrix.yx = 0;
			matrix.yy = 0x10000L;
			FT_Outline_Transform(&(m_face->glyph->outline), &matrix);
		}
		
		if ((uiStyle & CRenderFont::FS_BOLD) == CRenderFont::FS_BOLD)
		{
			FT_Outline_Embolden(&(m_face->glyph->outline), 1 << 6);
		}
		
		if (FT_Render_Glyph(m_face->glyph,  FT_RENDER_MODE_NORMAL))
		{
			return NULL;
		}

		pCacheGlyph = new CCacheGlyph(m_face->glyph, &(m_face->glyph->metrics));
		m_cachePool.addCacheObject(cacheGlyphKey, pCacheGlyph);

		return pCacheGlyph;
	}
}