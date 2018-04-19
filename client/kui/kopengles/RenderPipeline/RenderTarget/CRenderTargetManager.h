#pragma once

namespace OpenHMI
{
	class CRenderTargetManager : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static	CRenderTargetManager*	getInstance();
		static	void					deleteInstance();

		CRenderTargetManager();
		virtual ~CRenderTargetManager();

		void			initialize();
		void			unInitialize();
		void			clear();

		CRenderTarget*	getRenderTarget(UINT uiWidth, UINT uiHeight);

	protected:// method
		CRenderTargetManager(const CRenderTargetManager&)	{}
		CRenderTargetManager &operator = (const CRenderTargetManager &){return *this;}

	private:// method
		CRenderTarget*	getFreeRenderTarget(UINT uiWidth, UINT uiHeight, const String& strName);
		void			addRenderTarget(CRenderTarget* pRT);

	protected:// property

	private:// property
		static CRenderTargetManager*	s_pInstance;

		//CachePool<String, tagStringCompare>	m_cachePool;

		Array2<ObjectPtr<CRenderTarget> >	m_aryRenderTarget;
		//Map<INT, CRenderTarget, tagNumCompare<INT> >	m_mapRenderTarget;

		UINT		m_uiArySize;
	};
}