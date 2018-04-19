#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheProgram::CCacheProgram(GLuint uiID)
		: m_uiProgramID(uiID)
		, m_uiSamplerNum(0)
	{
		;
	}

	CCacheProgram::~CCacheProgram()
	{
		if (m_uiProgramID != 0)
		{
			changeRenderContext();
			glDeleteProgram( m_uiProgramID );
			m_uiProgramID = 0;
			resetRenderContext();
		}
	}

	void CCacheProgram::begin()
	{
		glUseProgram(m_uiProgramID);
	}

	void CCacheProgram::setUniforms(Object* p)
	{
		for( UINT i = 0; i< m_aryUniform.size() ; ++i )
		{
			const tagUniformInstance& rInstance = m_aryUniform[i];
			INT iLoc = m_aryLoc[i];

			rInstance.m_typeCallBackFunc(p, rInstance.m_nameCallBackFunc, iLoc);
		}
	}

	void CCacheProgram::setTextureUniforms(CPass* p)
	{
		if ( p == NULL )
		{
			return;
		}

		for( UINT i = 0; i < m_uiSamplerNum; ++i )
		{
			CCacheTexture* pTex = p->getTexture(i);
			if( pTex == NULL )
			{
				return;
			}

			switch(i)
			{
				case 0:
					glActiveTexture(GL_TEXTURE0);
					break;
				case 1:
					glActiveTexture(GL_TEXTURE1);
					break;
				case 2:
					glActiveTexture(GL_TEXTURE2);
					break;
				case 3:
					glActiveTexture(GL_TEXTURE3);
					break;
				case 4:
					glActiveTexture(GL_TEXTURE4);
					break;
				case 5:
					glActiveTexture(GL_TEXTURE5);
					break;
				case 6:
					glActiveTexture(GL_TEXTURE6);
					break;
				case 7:
					glActiveTexture(GL_TEXTURE7);
					break;
			}

			glBindTexture(GL_TEXTURE_2D, pTex->getTexID());
			p->applyTextureParameters(i,pTex);
		}
	}

	void CCacheProgram::end(CPass* p)
	{
		glUseProgram(0);

		for( UINT i = 0; i < m_uiSamplerNum; ++i )
		{
			CCacheTexture* pTex = p->getTexture(i);
			if( pTex == NULL )
			{
				return;
			}

			switch(i)
			{
				case 0:
					glActiveTexture(GL_TEXTURE0);
					break;
				case 1:
					glActiveTexture(GL_TEXTURE1);
					break;
				case 2:
					glActiveTexture(GL_TEXTURE2);
					break;
				case 3:
					glActiveTexture(GL_TEXTURE3);
					break;
				case 4:
					glActiveTexture(GL_TEXTURE4);
					break;
				case 5:
					glActiveTexture(GL_TEXTURE5);
					break;
				case 6:
					glActiveTexture(GL_TEXTURE6);
					break;
				case 7:
					glActiveTexture(GL_TEXTURE7);
					break;
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void CCacheProgram::addUniform(INT iLoc,const tagUniformInstance& instance)
	{
		m_aryUniform.addItem(instance);
		m_aryLoc.addItem(iLoc);
	}
}