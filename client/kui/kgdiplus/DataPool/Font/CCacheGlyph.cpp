#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheGlyph::CCacheGlyph(const FT_GlyphSlot glyphSlot, const FT_Glyph_Metrics* pMetrics)
	{
		memset(&m_metrics, 0x0, sizeof(m_metrics));

		FT_Bitmap_New(&m_bitmap);			
		if (glyphSlot != NULL)
		{
			FT_Bitmap_Copy(CFontPool::getInstance()->getFontLib(), &(glyphSlot->bitmap), &m_bitmap); 
		}

		if (pMetrics)
		{
			m_metrics = *pMetrics;
			if (glyphSlot != NULL)
			{
				m_metrics.horiBearingX = glyphSlot->bitmap_left << 6;
				m_metrics.horiBearingY = glyphSlot->bitmap_top << 6;
			}
		}	
	}

	CCacheGlyph::~CCacheGlyph()
	{
		FT_Bitmap_Done(CFontPool::getInstance()->getFontLib(), &m_bitmap);
	}
}