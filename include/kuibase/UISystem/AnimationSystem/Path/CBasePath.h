/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBasePath : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBasePath();
		
		virtual	FLOAT			getLength();
		virtual	CAmtPoint*		getPointByPercent(FLOAT fPercent);
		virtual	void			calculateEdgeLength();
		virtual	void			smashToSegments(INT iSegmentCount) = 0;

	protected:// method
		CBasePath();

	private:// method

	protected:// property
		Array2<INT>		m_aryX;
		Array2<INT>		m_aryY;

	private:// property
		Array2<FLOAT>	m_aryEdge;
		FLOAT			m_fLength;

		CAmtPoint*		m_pAmtPoint;
	};
}