#include "RenderEngine.h"
#include "./Shader/CInternalShaderLibrary.h"

namespace OpenHMI
{
	CShaderPool* CShaderPool::s_pInstance	= NULL;

	CShaderPool*	CShaderPool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CShaderPool();
		}

		return s_pInstance;
	}

	void CShaderPool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CShaderPool::CShaderPool()
		/*: m_pInternalShaderLibrary(NULL)*/
	{
		;
	}

	CShaderPool::~CShaderPool()
	{
		;
	}

	void CShaderPool::initialize()
	{
		m_cachePoolVertexShader.setPoolSize(CONFIG::getShaderCacheCount());
		m_cachePoolFragmentShader.setPoolSize(CONFIG::getShaderCacheCount());
	}

	void CShaderPool::unInitialize()
	{
		clear();
	}

	void CShaderPool::clear()
	{
		m_cachePoolVertexShader.clear();
		m_cachePoolFragmentShader.clear();
	}

	CCacheVertexShader* CShaderPool::getVertexShader(const String& strName)
	{
		CCacheVertexShader* p = m_cachePoolVertexShader.getCacheObject(strName);
		return p;
	}

	CCacheFragmentShader* CShaderPool::getFragmentShader(const String& strName)
	{
		CCacheFragmentShader* p = m_cachePoolFragmentShader.getCacheObject(strName);
		return p;
	}

	CCacheVertexShader* CShaderPool::getInternalVertexShader(EVertexShaderName eName)
	{
		String strName;
		indexToName(strName, L"InternalVertexShader", eName);
		
		CCacheVertexShader* p = m_cachePoolVertexShader.getCacheObject(String::toString(g_uiSessionID) + strName);
		if ( p == NULL )
		{
			p = new CCacheVertexShader();
			p->setName(String::toString(g_uiSessionID)+strName);
			m_cachePoolVertexShader.addCacheObject(String::toString(g_uiSessionID) + strName, p);
			
			// construct
			g_contructVertexShader(eName, p);
		}

		return p;
	}

	CCacheFragmentShader* CShaderPool::getInternalFragmentShader(EFragmentShaderName eName)
	{
		String strName;
		indexToName(strName, L"InternalFragmentShader", eName);
		
		CCacheFragmentShader* p = m_cachePoolFragmentShader.getCacheObject(String::toString(g_uiSessionID) + strName);
		if ( p == NULL )
		{
			p = new CCacheFragmentShader();
			p->setName(String::toString(g_uiSessionID)+strName);
			m_cachePoolFragmentShader.addCacheObject(String::toString(g_uiSessionID) + strName, p);

			// construct
			g_contructFragmentShader(eName, p);
		
		}

		return p;
	}

	String CShaderPool::getInternalVertexShaderName(EVertexShaderName eName)
	{
		String strName;
		indexToName(strName, L"InternalVertexShader", eName);
		return String::toString(g_uiSessionID) + strName;
	}

	String  CShaderPool::getInternalFragmentShaderName(EFragmentShaderName eName)
	{
		String strName;
		indexToName(strName, L"InternalFragmentShader", eName);
		return String::toString(g_uiSessionID) + strName;
	}

	void CShaderPool::freeVertexShader(CCacheVertexShader* pShader)
	{
		;
	}

	void CShaderPool::freeFragmentShader(CCacheFragmentShader* pShader)
	{
		;
	}

	GLuint CShaderPool::loadShader(GLenum eType, const char *pcShaderSrc)
	{
		GLuint	uiShader;
		GLint	iCompiled;

		// Create the shader object
		uiShader = glCreateShader( eType );
		if ( uiShader == 0 )
		{
			return 0;
		}

		// Load the shader source
		glShaderSource( uiShader, 1, &pcShaderSrc, NULL );

		// Compile the shader
		glCompileShader( uiShader );

		// Check the compile status
		glGetShaderiv( uiShader, GL_COMPILE_STATUS, &iCompiled );

		if ( !iCompiled ) 
		{
			GLint iInfoLen = 0;

			glGetShaderiv( uiShader, GL_INFO_LOG_LENGTH, &iInfoLen );

			if ( iInfoLen > 1 )
			{
				ArrayPtr<char> ptrInfoLog(new char [iInfoLen], iInfoLen);

				glGetShaderInfoLog( uiShader, iInfoLen, NULL, (char*)ptrInfoLog );

				String strInfoLog;
				String::convertFromMBCS((char*)ptrInfoLog, strInfoLog);
				CLog::error2(RENDER_ENGINE_NAME, (const CHAR*)strInfoLog);
			}

			glDeleteShader( uiShader );

			return 0;
		}

		return uiShader;
	}

	void CShaderPool::indexToName(String& strName, const CHAR* sz, UINT idx)
	{
		strName = String::format(L"%s %u",sz, idx);
	}
}