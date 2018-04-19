/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CViewProgressbar : public CView
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CViewProgressbar();
		virtual ~CViewProgressbar();

		void		setSkinRegularBar(UINT uiImageID);
		void		setSkinRegularSlider(UINT uiImageID);
		void		setSkinIrregularBar(UINT uiImageID);
		void		setSkinIrregularSlider(UINT uiImageID);

		CMovieClip*	getSkinRegularBar();
		CMovieClip*	getSkinRegularSlider();
		CMovieClip*	getSkinIrregularBar();
		CMovieClip*	getSkinIrregularSlider();

		void		drawSkinSlider(CMovieClip* pMovieClip, BOOLEAN bFocus);
		void		drawSkinIrregularBar(CMovieClip* pMovieClip, FLOAT fPosRatio);

	protected:// method
		CViewProgressbar(const CViewProgressbar&){}
		CViewProgressbar& operator =(const CViewProgressbar&){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT	m_uiRegularBarImageID;
		UINT	m_uiRegularSliderImageID;		
		UINT	m_uiIrregularBarImageID;
		UINT	m_uiIrregularSliderImageID;
	};
}