#pragma once

namespace OpenHMI
{
	class CProgramPool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		struct tagUniform
		{
			EUniformType	m_eType;
			EUniformName	m_eName;
			tagUniform() 
				: m_eType(EUNTYPE_NUM)
				, m_eName(EUNNAME_NUM)
			{
				;
			}

			tagUniform(EUniformType eType,EUniformName eName) 
				: m_eType(eType)
				, m_eName(eName)
			{
				;
			}
		};

	private:// embed class or struct or enum


	public:// method	
		static	CProgramPool*	getInstance();
		static	void			deleteInstance();

		void		initialize();
		void		unInitialize();
		void		clear();

		CCacheProgram*		getProgram(const String& strVpName, const String& strFpName);
		CCacheProgram*		getProgram(CShaderPool::EVertexShaderName eVSName, CShaderPool::EFragmentShaderName eFSName);

		void				freeProgram(CCacheProgram* pProgram);

	protected:// method	
		CProgramPool();
		virtual ~CProgramPool();

	private:// method
		void				initializeUniform();
		CCacheProgram*		generateProgram(CCacheVertexShader* pVS, CCacheFragmentShader* pFS);
		BOOLEAN				linkProgram(GLuint uiProgram);
		void				bindAttributes(UINT uiID, CCacheVertexShader* pVS);
		void				generateUniforms(CCacheProgram* pProgram, CCacheVertexShader* pVS, CCacheFragmentShader* pFS);

	protected:// property

	private:// property
		static	CProgramPool	*s_pInstance;

		CachePool<String, CCacheProgram, tagStringCompare>					m_cachePool;

		Map2<String, tagUniform, tagStringCompare>	m_mapUniform;
		Array2<Uniform_NameCallBackFunc>			m_aryUniformNameCallBackFunc;
		Array2<Uniform_TypeCallBackFunc>			m_aryUniformTypeCallBackFunc;
	};
}