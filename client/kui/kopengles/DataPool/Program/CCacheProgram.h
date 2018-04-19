#pragma once

namespace OpenHMI
{
	class CCacheProgram : public CacheGPUObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		struct tagUniformInstance
		{
			Uniform_NameCallBackFunc	m_nameCallBackFunc;
			Uniform_TypeCallBackFunc	m_typeCallBackFunc;

			tagUniformInstance()
				: m_nameCallBackFunc(NULL)
				, m_typeCallBackFunc(NULL)
			{
				;
			}
		};

	private:// embed class or struct or enum

	public:// method	
		CCacheProgram(GLuint uiID);
		virtual ~CCacheProgram();

		void	begin();
		void	setUniforms(Object* p);
		void	setTextureUniforms(CPass* p);
		void	end(CPass* p);

		void	addUniform(INT iLoc, const tagUniformInstance& instance);
		void	setSamplerNum(UINT uiNum)			{ m_uiSamplerNum = uiNum;	}

		UINT	getProgramID() const				{ return m_uiProgramID;	}
		
		const Array2<INT>& getUniformLoc()	const	{ return m_aryLoc;		}

	protected:// method
		CCacheProgram(const CCacheProgram &){}
		CCacheProgram &operator = (const CCacheProgram &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		GLuint						m_uiProgramID;

		Array2<INT>					m_aryLoc;
		Array2<tagUniformInstance>	m_aryUniform;
		UINT						m_uiSamplerNum;

		String						m_strVertexShaderName;
		String						m_strFragmentShaderName;
		
	};

	typedef ObjectPtr<CCacheProgram> CCacheProgramPtr;
}