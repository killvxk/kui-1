#pragma once

namespace OpenHMI
{
	class CCacheShader : public CacheGPUObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCacheShader();
		virtual ~CCacheShader();

		UINT		getShaderID() const			{ return m_uiShaderID; }

		const char*	getShader() const			{ return m_pszShader;  }
		void		setShader(const char* psz)	{ m_pszShader = (char*)psz;	}

		void		compile(GLenum eType);

	protected:// method
		CCacheShader(const CCacheShader &){}
		CCacheShader &operator = (const CCacheShader &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		UINT		m_uiShaderID;
		char*		m_pszShader;
	};

	typedef ObjectPtr<CCacheShader> CCacheShaderPtr;
}