/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CView : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CView();
		virtual ~CView();

		void		setSkin(UINT uiImageID)	{ m_uiImageID = uiImageID; }
		CMovieClip*	getSkin();

	protected:// method
		CView(const CView&){}
		CView& operator =(const CView&){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT	m_uiImageID;
	};
}