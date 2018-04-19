#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheShader::CCacheShader()
		: m_uiShaderID(0)
		, m_pszShader(NULL)
	{
		;
	}

	CCacheShader::~CCacheShader()
	{
		if (m_uiShaderID != 0)
		{
			changeRenderContext();
			glDeleteShader( m_uiShaderID );
			m_uiShaderID = 0;
			resetRenderContext();
		}
	}

	void CCacheShader::compile(GLenum eType)
	{
		GLuint	uiShader;
		GLint	iCompiled;

		// Create the shader object
		uiShader = glCreateShader( eType );
		if ( uiShader == 0 )
		{
			return;
		}

		// Load the shader source
		glShaderSource( uiShader, 1, (const char**)&m_pszShader, NULL );

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

			return ;
		}

		m_uiShaderID = uiShader;
	}
}