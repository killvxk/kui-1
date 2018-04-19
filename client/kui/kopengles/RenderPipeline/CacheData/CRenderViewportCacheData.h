#pragma once

#include "./RenderPipeline/C3DBatchRenderGroup.h"

namespace OpenHMI
{
	class CRenderViewportCacheData : public CMaterialRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderViewportCacheData(CRenderViewport* pRndViewport);
		virtual ~CRenderViewportCacheData();

		virtual	void		notifySizePropertyChanged()			{ m_bPropertyChanged = TRUE; }
		virtual	void		notifyMergePropertyChanged()		{ m_bPropertyChanged = TRUE; }
		virtual	void		notifyContentPropertyChanged()		{ m_bPropertyChanged = TRUE; }
		virtual void		flush();

		void				addWaver(CRenderEntry* pOwner);

	protected:// method
		CRenderViewportCacheData(const CRenderViewportCacheData&)	{}
		CRenderViewportCacheData &operator = (const CRenderViewportCacheData &){return *this;}

		virtual void	flushMaterial();
		void			flushWaver();

	private:// method

	protected:// property

	private:// property
		CRenderViewport*				m_pRndViewport;	
		C3DBatchRenderGroupPtr			m_p3DBatchRenderGroup;
		CRenderModelCacheData*			m_pRenderModelCacheData;
		BOOLEAN							m_bPropertyChanged;
	};
}