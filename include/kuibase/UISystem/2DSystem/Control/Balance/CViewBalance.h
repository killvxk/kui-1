/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CViewBalance : public CView
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CViewBalance();
		virtual ~CViewBalance();		

		void		 setSkinVerticalLine(UINT uiImageID);
		void		 setSkinHorizontalLine(UINT uiImageID);
		void		 setSkinSlider(UINT uiImageID);

		CMovieClip*  getSkinVerticalLine();
		CMovieClip*  getSkinHorizontalLine();
		CMovieClip*  getSkinSlider();  

		void		 drawSkin(CMovieClip *pMovieClip, BOOLEAN bFocus);

	protected:// method
		CViewBalance(const CViewBalance&){}
		CViewBalance& operator =(const CViewBalance&){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT	m_uiVLineImageID;
		UINT	m_uiHLineImageID;
		UINT	m_uiSliderImageID;
	};
}