#pragma once

namespace OpenHMI
{
	class CCacheVertexShader : public CCacheShader
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CCacheVertexShader();
		virtual ~CCacheVertexShader();

		inline const char*	getAttibute(UINT uidx)	const
		{ 
			if( m_ppszAttribute ) 
			{
				return m_ppszAttribute[uidx]; 
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

		UINT	getAttibuteNum() const	{ return m_uiAttNum;		}
		UINT	getUniformNum()	const	{ return m_uiUniformNum;	}

		void	setProperty(char** ppszAttr, UINT uiAttNum, char** ppszUniform, UINT uiUniformNum);

	protected:// method
		//CCacheVertexShader(const CCacheVertexShader &){}
		//CCacheVertexShader &operator = (const CCacheVertexShader &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		char**	m_ppszAttribute;
		UINT	m_uiAttNum;
		char**	m_ppszUniform;
		UINT	m_uiUniformNum;
	};

	typedef ObjectPtr<CCacheVertexShader> CCacheVertexShaderPtr;
}