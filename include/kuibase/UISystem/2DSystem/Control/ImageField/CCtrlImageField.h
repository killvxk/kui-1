/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CCtrlImageField : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlImageField();
		virtual ~CCtrlImageField();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set a user movie clip to this image field. The method is used to directly show some image file which comes from file system.
		*/
		void	setUserMovieClip(CMovieClip* pMovieClip);

		/**
		* Get the source image width.
		*/
		UINT	getSrcImageWidth();

		/**
		* Get the source image height.
		*/
		UINT	getSrcImageHeight();

		/**
		* Adjust the image field's preferred size to the source image size.
		*/
		void	adjustPreferredSizeToSrcImageSize();

		/**
		* @brief	Show image by index.
		*
		* @param	uiIndex It starts from 1.
		*/
		void	showImage(UINT uiIndex);

		/**
		* Play all the images in this image field with a interval of uiIntervalTime
		*/
		void	play(UINT uiIntervalTime);		

		/**
		* Stop playing images
		*/
		void	stop();
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set layout property to this image field. All the images in this image field will show using this layout style
		*/
		void					setLayout(CRenderImage::ELayout eValue);
		CRenderImage::ELayout	getLayout() const					{ return m_renderImage.getLayout(); }

		void					setNormalLayoutClipOffsetX(INT iValue);
		INT						getNormalLayoutClipOffsetX() const	{ return m_renderImage.getNormalLayoutClipOffsetX(); }

		void					setNormalLayoutClipOffsetY(INT iValue);
		INT						getNormalLayoutClipOffsetY() const	{ return m_renderImage.getNormalLayoutClipOffsetY(); }

		virtual	void	refresh();

	protected:// method
		CCtrlImageField(const CCtrlImageField&){}
		CCtrlImageField& operator =(const CCtrlImageField&){return *this;}

	private:// method

	protected:// property

	private:// property
		CView		m_view;
		ObjectPtr<CMovieClip>	m_userSkin;

		UINT		m_uiShowImage;
		UINT		m_uiIntervalTime;
	};
}