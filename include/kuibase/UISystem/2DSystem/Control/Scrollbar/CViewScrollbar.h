/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CViewScrollbar : public CView
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CViewScrollbar();
		virtual ~CViewScrollbar();

		void		setSkinUpArrow(UINT uiImageID);
		void		setSkinDownArrow(UINT uiImageID);
		void		setSkinBar(UINT uiImageID);
		void		setSkinSlider(UINT uiImageID);

		CMovieClip* getSkinUpArrow();
		CMovieClip* getSkinDownArrow();
		CMovieClip* getSkinBar();
		CMovieClip* getSkinSlider();

		void		drawSkinUpArrow(CMovieClip * pMovieClip, BOOLEAN bFocus);
		void		drawSkinDownArrow(CMovieClip * pMovieClip, BOOLEAN bFocus);
		void		drawSkinBar(CMovieClip * pMovieClip, BOOLEAN bFocus);
		void		drawSkinSlider(CMovieClip * pMoiveClip, BOOLEAN bFocus);

	protected:// method
		CViewScrollbar(const CViewScrollbar&){}
		CViewScrollbar& operator =(const CViewScrollbar&){return *this;}

	private:// method
		 void drawSkin(CMovieClip * pMoiveClip, BOOLEAN bFocus);

	protected:// property

	private:// property
		UINT	m_uiUpArrowImageID;
		UINT	m_uiDownArrowImageID;
		UINT	m_uiBarImageID;
		UINT	m_uiSliderImageID;
	};
}