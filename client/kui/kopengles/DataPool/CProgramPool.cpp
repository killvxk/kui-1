#include "RenderEngine.h"

namespace OpenHMI
{
	CProgramPool* CProgramPool::s_pInstance	= NULL;

	CProgramPool*	CProgramPool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CProgramPool();
		}

		return s_pInstance;
	}

	void CProgramPool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CProgramPool::CProgramPool()
		: m_aryUniformNameCallBackFunc(EUNTYPE_NUM)
		, m_aryUniformTypeCallBackFunc(EUNNAME_NUM)
	{
		;
	}

	CProgramPool::~CProgramPool()
	{
		m_mapUniform.clear();
		m_aryUniformNameCallBackFunc.clear();
		m_aryUniformTypeCallBackFunc.clear();
	}

	void CProgramPool::initialize()
	{
		m_cachePool.setPoolSize(CONFIG::getProgramCacheCount());
		initializeUniform();
	}

	void CProgramPool::unInitialize()
	{
		clear();
	}

	void CProgramPool::clear()
	{
		m_cachePool.clear();
	}

	CCacheProgram* CProgramPool::getProgram(const String& strVpName, const String& strFpName)
	{
		return NULL;
	}

	CCacheProgram* CProgramPool::getProgram(CShaderPool::EVertexShaderName eVSName, CShaderPool::EFragmentShaderName eFSName)
	{
		String strVertexShaderName = CShaderPool::getInstance()->getInternalVertexShaderName(eVSName);
		String strFragmentShaderName = CShaderPool::getInstance()->getInternalFragmentShaderName(eFSName);
		
		CCacheProgram* p = m_cachePool.getCacheObject(String::toString(g_uiSessionID) + strVertexShaderName + strFragmentShaderName);
		if ( p== NULL )
		{	
			CCacheVertexShaderPtr pVertexShader(CShaderPool::getInstance()->getInternalVertexShader(eVSName) );
			CCacheFragmentShaderPtr pFragmentShader(CShaderPool::getInstance()->getInternalFragmentShader(eFSName) );
			p = generateProgram(pVertexShader, pFragmentShader);
			p->setName(String::toString(g_uiSessionID) + pVertexShader->getName() + pFragmentShader->getName());
			p->setRenderContext(g_pRenderContext);
			m_cachePool.addCacheObject(String::toString(g_uiSessionID) + pVertexShader->getName() + pFragmentShader->getName(),p);	
		}

		return p;
	}

	void CProgramPool::freeProgram(CCacheProgram* pProgram)
	{
		;
	}

	void  CProgramPool::initializeUniform()
	{
#define ADDUNIFORM(strName, eType, eName, func)						\
		m_mapUniform.addItem(strName, tagUniform(eType,eName) );	\
		m_aryUniformNameCallBackFunc.addItem((void*)func);
		
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Int);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_IntArray);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Float);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_FloatArray);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Vec3);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Vec3Array);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Vec4);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Vec4Array);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Matrix3);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Matrix3Array);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Matrix4);
		m_aryUniformTypeCallBackFunc.addItem(setUniform_Matrix4Array);

		ADDUNIFORM(L"u_uimatrix", EUNTYPE_MATRIX4, EUNNAME_PROJECT_UI, &getProject_UI);
		ADDUNIFORM(L"u_uiMPmatrix", EUNTYPE_MATRIX4, EUNNAME_MP_UI, &getMP_UI);
	}

	CCacheProgram* CProgramPool::generateProgram(CCacheVertexShader* pVS, CCacheFragmentShader* pFS)
	{
		// Create the program object
		GLuint uiID = glCreateProgram( );

		if ( uiID == 0 )
		{
			CLog::info(String(RENDER_ENGINE_NAME), String(L"CProgramPool generateProgram ERROR glCreateProgram failed"));
			return NULL;
		}

		bindAttributes(uiID, pVS);

		glAttachShader( uiID, pVS->getShaderID() );
		glAttachShader( uiID, pFS->getShaderID() );

		if (linkProgram(uiID) == FALSE)
		{
			CLog::info(String(RENDER_ENGINE_NAME), String(L"linkProgram failed") + pVS->getName() + pFS->getName() );
			return NULL;
		}
		
		CCacheProgram* pProgram = new CCacheProgram(uiID);
		glUseProgram(uiID);
		generateUniforms(pProgram, pVS, pFS);
		glUseProgram(0);

		return pProgram;
	}

	BOOLEAN CProgramPool::linkProgram(GLuint uiProgram)
	{
		GLint	iLinked;

		// Link the program
		glLinkProgram( uiProgram );

		// Check the link status
		glGetProgramiv( uiProgram, GL_LINK_STATUS, &iLinked );

		if ( !iLinked ) 
		{
			GLint iInfoLen = 0;

			glGetProgramiv( uiProgram, GL_INFO_LOG_LENGTH, &iInfoLen );

			if ( iInfoLen > 1 )
			{
				ArrayPtr<char> ptrInfoLog(new char [iInfoLen], iInfoLen);

				glGetProgramInfoLog( uiProgram, iInfoLen, NULL, (char*)ptrInfoLog );
				
				String strInfoLog;
				String::convertFromMBCS((char*)ptrInfoLog, strInfoLog);
				CLog::debug2(RENDER_ENGINE_NAME, (const CHAR*)strInfoLog);
			}

			glDeleteProgram( uiProgram );

			return FALSE;
		}

		return TRUE;
	}

	void CProgramPool::bindAttributes(UINT uiID, CCacheVertexShader* pVS)
	{
		INT i = 0 ; 

		//const CCacheVertexShader::CProperty& source = pVS->getProperty();
		for(UINT uidx = 0; uidx < pVS->getAttibuteNum(); ++uidx)
		{
			const char* psz = pVS->getAttibute(uidx);
			if( psz )
			{
				glBindAttribLocation(uiID, i, psz);
				++i;
			}
		}
	}

	void CProgramPool::generateUniforms(CCacheProgram* pProgram, CCacheVertexShader* pVS, CCacheFragmentShader* pFS)
	{
		UINT uiUniformIdx = 0;
		UINT uiPrgID = pProgram->getProgramID();

		//const CCacheVertexShader::CProperty& vpsource = pVS->getProperty();
		//const CCacheFragmentShader::CProperty& fpsource = pFS->getProperty();
		for ( UINT i = 0; i < pVS->getUniformNum(); ++i )
		{
			const char* psz = pVS->getUniform(i);
			if( psz == NULL )
			{
				CLog::error2(RENDER_ENGINE_NAME, L"generateUniforms vpsource.getUniform(i) error");
				return;
			}

			String strkey;
			String::convertFromMBCS(psz,strkey);
			tagUniform& rUniform =  m_mapUniform.getValueByKey(strkey);
			CCacheProgram::tagUniformInstance uniformInstance;

			if ( rUniform.m_eType != EUNTYPE_NUM && rUniform.m_eName != EUNNAME_NUM )
			{
				uniformInstance.m_nameCallBackFunc = m_aryUniformNameCallBackFunc[rUniform.m_eName];
				uniformInstance.m_typeCallBackFunc = m_aryUniformTypeCallBackFunc[rUniform.m_eType];
			}

			INT iLoc = glGetUniformLocation( uiPrgID, psz );
			pProgram->addUniform(iLoc, uniformInstance);

			++uiUniformIdx;
		}
		
		for ( UINT i = 0; i < pFS->getUniformNum(); ++i )
		{
			const char* psz = pFS->getUniform(i);
			if( psz == NULL )
			{
				CLog::error2(RENDER_ENGINE_NAME, L"generateUniforms fpsource.getUniform(i) error");
				return;
			}

			String strkey;
			String::convertFromMBCS(psz,strkey);
			tagUniform& rUniform =  m_mapUniform.getValueByKey(strkey);
			CCacheProgram::tagUniformInstance uniformInstance;

			if ( rUniform.m_eType != EUNTYPE_NUM && rUniform.m_eName != EUNNAME_NUM )
			{			
				uniformInstance.m_nameCallBackFunc = m_aryUniformNameCallBackFunc[rUniform.m_eName];
				uniformInstance.m_typeCallBackFunc = m_aryUniformTypeCallBackFunc[rUniform.m_eType];
			}

			INT iLoc = glGetUniformLocation( uiPrgID, psz );
			pProgram->addUniform(iLoc, uniformInstance);

			++uiUniformIdx;
		}

		pProgram->setSamplerNum(pFS->getSamplerNum());
		INT idx = 0;
		for ( UINT i = 0; i < pFS->getSamplerNum(); ++i )
		{
			const char* psz = pFS->getSampler(i);
			if( psz == NULL )
			{
				CLog::error2(RENDER_ENGINE_NAME, L"generateUniforms fpsource.getSampler(i) error");
				return;
			}

			INT iTexLoc = glGetUniformLocation(uiPrgID, psz );
			glUniform1i(iTexLoc, idx);

			++idx;
		}
	}
}