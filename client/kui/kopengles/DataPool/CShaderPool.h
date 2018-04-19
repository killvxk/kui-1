#pragma once

namespace OpenHMI
{
	class CShaderPool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EVertexShaderName
		{
			EVS_UI_DEFAULT,
			EVS_UI_ALPHA,
			EVS_WAVE,
			EVS_UI_VIEWPORT,
			EVS_25DUI_ALPHA,
			EVS_OPENARTIST_LINESOLID,
			EVS_GAUSSIAN_BLUR_H,
			EVS_GAUSSIAN_BLUR_V,
			EVS_UI_DEFAULT_MERGE,
			EVS_UI_MASK,
			EVS_UI_MASK2,

			EVS_NUM
		};

		enum EFragmentShaderName
		{
			EFS_UI_DEFAULT,
			EFS_UI_ALPHA,
			EFS_WAVE,
			EFS_UI_VIEWPORT,
			EFS_25DUI_ALPHA,
			EFS_OPENARTIST_LINESOLID,
			EFS_GAUSSIAN_BLUR_H,
			EFS_GAUSSIAN_BLUR_V,
			EFS_UI_DEFAULT_MERGE,
			EFS_UI_MASK,
			EFS_UI_MASK2,

			EFS_NUM
		};

	private:// embed class or struct or enum


	public:// method	
		static	CShaderPool*	getInstance();
		static	void			deleteInstance();

		void		initialize();
		void		unInitialize();
		void		clear();

		CCacheVertexShader*		getVertexShader(const String& strName);
		CCacheFragmentShader*	getFragmentShader(const String& strName);

		CCacheVertexShader*		getInternalVertexShader(EVertexShaderName eName);
		CCacheFragmentShader*	getInternalFragmentShader(EFragmentShaderName eName);

		String					getInternalVertexShaderName(EVertexShaderName eName);
		String					getInternalFragmentShaderName(EFragmentShaderName eName);

		void					freeVertexShader(CCacheVertexShader* pShader);
		void					freeFragmentShader(CCacheFragmentShader* pShader);

	protected:// method	
		CShaderPool();
		virtual ~CShaderPool();

	private:// method
		GLuint		loadShader(GLenum eType, const char *pcShaderSrc);
		void		indexToName(String& strName, const CHAR* sz, UINT idx);
		
	protected:// property

	private:// property
		static	CShaderPool	*s_pInstance;

		CachePool<String, CCacheVertexShader, tagStringCompare>		m_cachePoolVertexShader;
		CachePool<String, CCacheFragmentShader, tagStringCompare>	m_cachePoolFragmentShader;
	};
}