#pragma once

namespace OpenHMI
{
	class CBaseFilter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CBaseFilter()
			: m_bIsNeedRT(TRUE)
			, m_bGenerateRT(FALSE)
		{
			;
		}

		virtual ~CBaseFilter(){}		

		/**
		* @brief	generate some properties related to render from the CRenderFilter of CRenderGroup
		* @parma	pRndGroupCacheData	this is the cache data of the CRenderGroup
		* @parma	pFilter				this is the CRenderFilter of CRenderGroup
		*/
		virtual BOOLEAN fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData,const CRenderBaseFilter* pFilter);

		/**
		* @brief	dofilter
		* @parma	pIn				this is the source RT
		* @parma	pbg				this is the background RT
		* @parma	pOut			this is the out RT.
		* @parma	bHasNextFilter	this shows whether this filter is the last filter of the CRenderGroup or not
		*/
		virtual void	doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut);
		virtual void	doFilter(C3DBatchRenderGroup* pIn, C3DBatchRenderGroup* pbg, Array2<C3DBatchRenderGroup*>& aryOut);

		BOOLEAN			isNeedRT() const				{ return m_bIsNeedRT;		}
		void			setGenerateRT(BOOLEAN bValue)	{ m_bGenerateRT = bValue;	}
		BOOLEAN			isGenerateRT() const			{ return m_bGenerateRT;		}

	protected:// method
		CBaseFilter(const CBaseFilter&){}
		CBaseFilter& operator =(const CBaseFilter&){return *this;}

	private:// method

	protected:// property
		BOOLEAN		m_bIsNeedRT;
		
	private:// property
		BOOLEAN		m_bGenerateRT;
	};
}