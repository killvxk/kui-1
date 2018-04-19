#pragma once

namespace OpenHMI
{
	class CRenderImageCacheData : public CMaterialRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderImageCacheData(CRenderImage* pRndImage);
		virtual ~CRenderImageCacheData();

		virtual	void		notifyContentPropertyChanged();
		virtual void		flush(/*CCacheImage* pImage*/);

		CCacheTexture*		getTexture();
		CDIB*				getDIB()						{ return m_pDIB;	}

	protected:// method
		CRenderImageCacheData(const CRenderImageCacheData&)	{}
		CRenderImageCacheData &operator = (const CRenderImageCacheData &){return *this;}	

		virtual void	flushMaterial();

	private:// method

	protected:// property

	private:// property	
		//CCacheTexture	*m_pTex;
		//tagResourceLoc	m_iTexID;
		CCacheTexturePtr	m_pTex;
		CDIB				*m_pDIB;
		CRenderImage		*m_pRndImage;
		BOOLEAN				m_bPropertyChanged;
		
	};
}