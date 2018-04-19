#pragma once

namespace OpenHMI
{
	class CCacheFragmentShader : public CCacheShader
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CCacheFragmentShader();
		virtual ~CCacheFragmentShader();

		inline const char*	getSampler(UINT uidx) const
		{ 
			if( m_ppszSampler ) 
			{
				return m_ppszSampler[uidx]; 
			}

			return NULL;	
		}

		inline const char*	getUniform(UINT uidx) const
		{ 
			if( m_ppszUniform ) 
			{
				return m_ppszUniform[uidx]; 
			}

			return NULL;	
		}

		UINT	getSamplerNum() const	{ return m_uiSamplerNum;	}
		UINT	getUniformNum()	const	{ return m_uiUniformNum;	}

		void	setProperty(char** ppszSampler, UINT uiSamplerNum, char** ppszUniform, UINT uiUniformNum);

	protected:// method
		
	private:// method

	protected:// property

	private:// property	
		char**	m_ppszSampler;
		UINT	m_uiSamplerNum;
		char**	m_ppszUniform;
		UINT	m_uiUniformNum;
	};

	typedef ObjectPtr<CCacheFragmentShader> CCacheFragmentShaderPtr;
}