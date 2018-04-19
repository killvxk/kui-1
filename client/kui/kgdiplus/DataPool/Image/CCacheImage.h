#pragma once

namespace OpenHMI
{
	class CCacheImage : public CacheObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCacheImage(UINT uiWidth, UINT uiHeihgt, BYTE *pbtPixelData);
		virtual ~CCacheImage();

		UINT		getWidth() const		{ return m_uiWidth; }
		UINT		getHeight() const		{ return m_uiHeihgt; }
		const BYTE*	getPixelData() const	{ return m_pbtPixelData; }

	protected:// method
		CCacheImage(const CCacheImage &){}
		CCacheImage &operator = (const CCacheImage &){return *this;}

	private:// method

	protected:// property

	private:// property	
		INT		m_uiWidth;
		INT		m_uiHeihgt;
		BYTE	*m_pbtPixelData;
	};

	typedef ObjectPtr<CCacheImage> CCacheImagePtr;
}