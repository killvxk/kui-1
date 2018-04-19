#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheFragmentShader::CCacheFragmentShader()
		: m_ppszSampler(NULL)
		, m_uiSamplerNum(0)
		, m_ppszUniform(NULL)
		, m_uiUniformNum(0)
	{
		;
	}

	CCacheFragmentShader::~CCacheFragmentShader()
	{
		;
	}

	void CCacheFragmentShader::setProperty(char** ppszSampler, UINT uiSamplerNum, char** ppszUniform, UINT uiUniformNum)
	{
		m_ppszSampler = ppszSampler;
		m_uiSamplerNum = uiSamplerNum;
		m_ppszUniform = ppszUniform;
		m_uiUniformNum = uiUniformNum;
	}
}