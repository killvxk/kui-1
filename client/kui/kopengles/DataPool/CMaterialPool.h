#pragma once

namespace OpenHMI
{
	class CMaterialPool
	{
	public:// const define
		
	private:// const define

	public:// embed class or struct or enum
		enum EMaterialType
		{
			EMT_UI_DEFAULT,
			EMT_UI_ALPHA,
			EMT_WAVE,
			EMT_25DUI_ALPHA,
			EMT_UI_VIEWPORT,
			EMT_OPENARTIST_LINESOLID,
			EMT_GAUSSIAN_BLUR_H,
			EMT_GAUSSIAN_BLUR_V,
			EMT_UI_DEFAULT_MERGE,
			EMT_UI_MASK,
			EMT_UI_MASK2,

			EMT_NUM
		};

		enum EPass_Type
		{
			EPT_INTERNAL_UI_COMMON,
			EPT_INTERNAL_MODEL_CUSTOM,

			EPT_NUM
		};

		struct tagTextureState
		{
			UINT	m_uiTexType;
			UINT	m_uiWrapMode;
			UINT	m_uiFilterMode;
			UINT	m_uiTexIdx;

			tagTextureState()
				: m_uiTexType(CCacheTexture::TEXTYPE_2D)
				, m_uiWrapMode(CCacheTexture::WRAPMODE_CLAMP_TO_EDGE)
				, m_uiFilterMode(CCacheTexture::FM_LINEAR)
				, m_uiTexIdx(0)
			{
				;
			}
		};

	private:// embed class or struct or enum


	public:// method	
		static	CMaterialPool*	getInstance();
		static	void			deleteInstance();

		void		initialize();
		void		unInitialize();
		void		clear();

		CCacheMaterial*	getExternalMaterial(const String& strFilePath);
		CCacheMaterial*	getInternalMaterial(EMaterialType eType, CCacheTexture* pTex);
		CCacheMaterial*	getInternalMaterial(EMaterialType eType, const Array2<CCacheTexture*>& aryTex);
		CCacheMaterial*	getInternalMergeMaterial();
		
		void			freeMaterial(CCacheMaterial* pMtl);

	protected:// method	
		CMaterialPool();
		virtual ~CMaterialPool();

	private:// method		
		String		generateInternalMtlName(EMaterialType eType, UINT iTexID);
		String		generateInternalMtlName(EMaterialType eType, const Array2<CCacheTexture*>& aryTex);
	
	protected:// property

	private:// property
		static	CMaterialPool	*s_pInstance;

		CachePool<String, CCacheMaterial, tagStringCompare>	m_cachePool;
	};
}