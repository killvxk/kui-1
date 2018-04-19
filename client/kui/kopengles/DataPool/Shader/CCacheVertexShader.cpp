#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheVertexShader::CCacheVertexShader()
		: m_ppszAttribute(NULL)
		, m_uiAttNum(0)
		, m_ppszUniform(NULL)
		, m_uiUniformNum(0)
	{
		;
	}

	CCacheVertexShader::~CCacheVertexShader()
	{
		;
	}

	void CCacheVertexShader::setProperty(char** ppszAttr, UINT uiAttNum, char** ppszUniform, UINT uiUniformNum)
	{
		m_ppszAttribute = ppszAttr;
		m_uiAttNum = uiAttNum;
		m_ppszUniform = ppszUniform;
		m_uiUniformNum = uiUniformNum;
	}
}