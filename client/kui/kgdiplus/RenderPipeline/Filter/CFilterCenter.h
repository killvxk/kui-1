#pragma once

namespace OpenHMI
{
	class CFilterCenter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		/**
		* @brief	apply filter effect on the source DIB, some filters maybe affect the background DIB at the same time.
		* @parma	srcDib this is the source DIB
		* @parma	bgDib	this is the background DIB
		* @parma	iBgX	x position which the source DIB lies on the background.
		* @parma	iBgY	y position which the source DIB lies on the background.
		* @parma	pRndFilter which contains the render params.
		*/
		static	void doFilter(CDIB &srcDib, CDIB &bgDib, INT &iBgX, INT &iBgY, const CRenderBaseFilter *pRndFilter);

	protected:// method
		CFilterCenter(){}
		CFilterCenter(const CFilterCenter&){}
		virtual ~CFilterCenter(){}		
		CFilterCenter& operator =(const CFilterCenter&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}