#pragma once

namespace OpenHMI
{
	class CCacheGlyph : public CacheObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCacheGlyph(const FT_GlyphSlot glyphSlot, const FT_Glyph_Metrics *pMetrics);
		virtual ~CCacheGlyph();

		const	FT_Bitmap*			getBitmap()	const	{ return &m_bitmap; }
		const	FT_Glyph_Metrics*	getMetrics() const	{ return &m_metrics; }

	protected:// method
		CCacheGlyph(const CCacheGlyph &){}
		CCacheGlyph &operator = (const CCacheGlyph &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		FT_Bitmap 			m_bitmap;
		FT_Glyph_Metrics	m_metrics;
	};

	typedef ObjectPtr<CCacheGlyph> CCacheGlyphPtr;
}