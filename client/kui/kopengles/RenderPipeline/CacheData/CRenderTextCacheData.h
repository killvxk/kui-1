#pragma once

namespace OpenHMI
{
	class CRenderTextCacheData : public CMaterialRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderTextCacheData();
		virtual ~CRenderTextCacheData();

		virtual	void	notifyContentPropertyChanged();
		virtual void	flush(CRenderText *pRndText);

		CCacheTexture*	getTexture()					{ return m_pTex;	}
		CDIB*			getDIB()						{ return m_pDIB;	}
		UINT			getLineHeight() const			{ return m_uiLineHeight; }
		BOOLEAN			isUseMergeTexture() const		{ return (m_pTex && m_pTex->isTextMergeTexture());	}
		CRectangleF		getTextureROI();
		

	protected:// method
		CRenderTextCacheData(const CRenderTextCacheData&)	{}
		CRenderTextCacheData &operator = (const CRenderTextCacheData &){return *this;}

		virtual void	flushMaterial();

	private:// method
		CDIB*	generateDIB(CRenderText* pRndText);
		BOOLEAN	generateMergeTexture();

	protected:// property

	private:// property	
		CCacheTexturePtr	m_pTex;
		//UINT				m_uiTexIdx;
		Array2<UINT>		m_aryTexIdx;
		CDIB				*m_pDIB;
		UINT				m_uiLineHeight;
		BOOLEAN				m_bPropertyChanged;
	};
}