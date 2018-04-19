/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CViewLabel : public CView
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CViewLabel();
		virtual ~CViewLabel();

		/**
		* @brief	draw skin of this button
		*
		*			this function is invoked by OpenHMI SDK,the project developer should not access this function
		*/
		void	drawSkin(CMovieClip *pMovieClip, BOOLEAN bEnable, BOOLEAN bHover, BOOLEAN bFocus, BOOLEAN bSelected, BOOLEAN bLongFocus);

	protected:// method
		CViewLabel(const CViewLabel&){}
		CViewLabel& operator =(const CViewLabel&){return *this;}

	private:// method

	protected:// property

	private:// property		

	};
}