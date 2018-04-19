#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheTexture::CCacheTexture()
		: m_uiWidth(0)
		, m_uiHeight(0)
		, m_uiFormat(TEXFMT_RGBA8888)
		, m_uiType(TEXTYPE_2D)
		, m_uiUsage(TEXUSAGE_NULL)
		, m_uiTexId(0)
		, m_wrap_s(WRAPMODE_REPEAT)
		, m_wrap_t(WRAPMODE_REPEAT)
		, m_wrap_r(WRAPMODE_REPEAT)
		, m_min_filter(FM_NEAREST)
		, m_mag_filter(FM_NEAREST)
		, m_bNeedUpdateParameter(TRUE)
	{
		;
	}

	CCacheTexture::~CCacheTexture()
	{
		if( m_uiTexId != 0 )
		{
			changeRenderContext();
			glDeleteTextures(1, &m_uiTexId);
			resetRenderContext();
		}
	}

	void CCacheTexture::generateGPUResource(const BYTE* pData)
	{
		// Generate a texture object
		glGenTextures(1, &m_uiTexId);

		// Bind the texture object
		glBindTexture(GL_TEXTURE_2D, m_uiTexId);		

		// Load the texture
		GLint internalFormat = GL_RGBA;
		GLenum type = 0;
		TransferTexFormat(m_uiFormat,internalFormat,type);		
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_uiWidth, m_uiHeight, 0, internalFormat, type, pData);

		GLenum error = glGetError();
		if ( error == GL_OUT_OF_MEMORY )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR Out of Memory CCacheTexture generate");
		}
		else if( error != GL_NO_ERROR )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR other error CCacheTexture generate");
		}

		applyTextureParameters();

		onGenerateGPUResource();
	}

	void CCacheTexture::fill(const BYTE *pData)
	{
		if ( pData )
		{
			glBindTexture(GL_TEXTURE_2D, m_uiTexId);
 			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_uiWidth, m_uiHeight, GL_RGBA, GL_UNSIGNED_BYTE, (BYTE*)pData);				
		}
	}

	void CCacheTexture::fill(INT uiXOffset, INT uiYOffset, INT uiWidth, INT uiHeight, const BYTE* pData)
	{
		if ( pData )
		{
			glBindTexture(GL_TEXTURE_2D, m_uiTexId);
 			glTexSubImage2D(GL_TEXTURE_2D, 0, uiXOffset, uiYOffset, uiWidth, uiHeight, GL_RGBA, GL_UNSIGNED_BYTE, (BYTE*)pData);	
		}
	}

	void CCacheTexture::TransferTexFormat(UINT format, GLint& internalFormat, GLenum& type)
	{
		switch ( format )
		{
		case TEXFMT_RGBA8888:
			internalFormat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case TEXFMT_RGB888:
			internalFormat = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case TEXFMT_RGBA5551:
			internalFormat = GL_RGBA;
			type = GL_UNSIGNED_SHORT_5_5_5_1;
			break;
		case TEXFMT_RGB565:
			internalFormat = GL_RGB;
			type = GL_UNSIGNED_SHORT_5_6_5;
			break;
		case TEXFMT_ALPHA:
			internalFormat = GL_ALPHA;
			type = GL_UNSIGNED_BYTE;
			break;
		case TEXFMT_LUMINANCE:
			internalFormat = GL_LUMINANCE;
			type = GL_UNSIGNED_BYTE;
			break;
		}
	}

	void CCacheTexture::setWrap(EWrapParameter eWhich, EWrapMode eWrap)
	{
		switch( eWhich )
		{
			case WRAP_S : 
				{
					if ( eWrap != m_wrap_s )
					{
						m_wrap_s = eWrap; 
						dirtyTextureParameters(); 
					}
				}
				break;
			case WRAP_T : 
				{
					if ( eWrap != m_wrap_t )
					{
						m_wrap_t = eWrap; 
						dirtyTextureParameters(); 
					}
				}
				break;
			case WRAP_R : 
				{
					if ( eWrap != m_wrap_r )
					{
						m_wrap_r = eWrap; 
						dirtyTextureParameters(); 
					}
				}
				break;
		}
	    
	}

	CCacheTexture::EWrapMode CCacheTexture::getWrap(EWrapParameter eWhich) const
	{
		switch( eWhich )
		{
			case WRAP_S : 
				return m_wrap_s;
			case WRAP_T : 
				return m_wrap_t;
			case WRAP_R : 
				return m_wrap_r;
		}
		return WRAPMODE_REPEAT;
	}

	void CCacheTexture::setFilter(EFilterParameter eWhich, EFilterMode eFilter)
	{
		switch( eWhich )
		{
			case FP_MIN_FILTER : 
				{
					if ( eFilter != m_min_filter )
					{
						m_min_filter = eFilter; 
						dirtyTextureParameters();
					}
				}
				break;
			case FP_MAG_FILTER : 
				{
					if ( eFilter != m_mag_filter )
					{
						m_mag_filter = eFilter; 
						dirtyTextureParameters(); 
					}
				}
				break;
		}
	}

	CCacheTexture::EFilterMode CCacheTexture::getFilter(EFilterParameter eWhich) const
	{
		switch( eWhich )
		{
			case FP_MIN_FILTER : 
				return m_min_filter;
			case FP_MAG_FILTER : 
				return m_mag_filter;
		}
		return FM_NEAREST;
	}

	void CCacheTexture::applyTextureParameters()
	{
		if ( !m_bNeedUpdateParameter )
			return;		

#ifdef OS_WIN
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)m_min_filter);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)m_mag_filter);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_t);
#endif

#ifdef OS_QNX
		// if powervr GPU the texture only support this wrap and filter if texture is not 2^N
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#endif

#ifdef OS_LINUX
		// if powervr GPU the texture only support this wrap and filter if texture is not 2^N
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
#endif 

		m_bNeedUpdateParameter = FALSE;
	}
}