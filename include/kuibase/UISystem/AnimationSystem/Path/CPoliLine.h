/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CPoliLine : public CBasePath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CPoliLine();
		virtual ~CPoliLine();		
		
		void addPoint(INT iX, INT iY);
		void addPoint(CAmtPoint* pAmtp);
		
		virtual	void smashToSegments(INT iSegmentCount);

	protected:// method
		CPoliLine(const CPoliLine&){}
		CPoliLine& operator =(const CPoliLine&){return *this;}

	private:// method

	protected:// property

	private:// property
		Array<CAmtPoint>	m_aryPoints;
	};
}