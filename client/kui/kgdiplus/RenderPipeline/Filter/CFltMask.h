#pragma once

namespace OpenHMI
{
	class CFltMask : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CFltMask();
		virtual ~CFltMask();

		void doFilter(CDIB &srcDib, CDIB &bgDib, INT &iBgX, INT &iBgY, const CRenderFltMask *pRndFilter);

	protected:// method
		CFltMask(const CFltMask&){}
		CFltMask& operator =(const CFltMask&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}