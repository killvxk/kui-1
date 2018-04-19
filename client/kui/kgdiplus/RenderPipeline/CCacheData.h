#pragma once

namespace OpenHMI
{
	class CCacheData : public CRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCacheData();
		virtual ~CCacheData();

		// size: width, height
		virtual	void	notifySizePropertyChanged();
		// : x, y, alpha, rotate, scale, etc
		virtual	void	notifyMergePropertyChanged();
		// content: text, font, image path, filter, etc
		virtual	void	notifyContentPropertyChanged();

		virtual void	notifyChildrenChanged();

		void	setDIB(CDIB *pDib);
		CDIB*	getDIB()			{ return m_pDib; }

		void	setLineHeight(UINT uiValue)	{ m_uiLineHeight = uiValue; }
		UINT	getLineHeight() const		{ return m_uiLineHeight; }
		

	protected:// method
		CCacheData(const CCacheData&)	{}
		CCacheData &operator = (const CCacheData &){return *this;}

	private:// method
		void	freeDIB();

	protected:// property

	private:// property
		CDIB	*m_pDib;

		UINT	m_uiLineHeight;
	};
}