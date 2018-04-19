#pragma once

namespace OpenHMI
{
	class CFltReflection : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CFltReflection();
		virtual ~CFltReflection();

		void doFilter(CDIB &srcDib, CDIB &bgDib, INT &iBgX, INT &iBgY, const CRenderFltReflection *pRndFilter);

	protected:// method
		CFltReflection(const CFltReflection&){}
		CFltReflection& operator =(const CFltReflection&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}