/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CPathGroup : public CBasePath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CPathGroup();
		virtual ~CPathGroup();
		
		INT					getPathCount();
		CBasePath*			getPath(UINT uiIndex);
		void				addPath(CBasePath* pPath);
		void				removePath(UINT uiIndex);
		void				clearAllPaths();

		virtual	FLOAT		getLength();
		virtual	CAmtPoint*	getPointByPercent(FLOAT fPercent);
		virtual	void		smashToSegments(INT iSegmentCount);

	protected:// method
		CPathGroup(const CPathGroup&){}
		CPathGroup& operator =(const CPathGroup&){return *this;}

	private:// method

	protected:// property

	private:// property
		Array<CBasePath>	m_aryListPath;
	};
}