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
	/**
	* @class	CMovieClip
	*			This class manages a serial images and provides control to this serial of images.
	*/
	class OPENHMI_API CMovieClip 
		: public Object
		, public ITimerListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		typedef struct tagBitmapInfo
		{
			String			strID;
			Gdiplus::Bitmap* pImage;
		}tagBitmapInfo;

		enum eImageType
		{
			TYPE_FILE = 0,
			TYPE_DATA,
		};

	private:// embed class or struct or enum
		


	public:// method
		CMovieClip();
		virtual ~CMovieClip();

		UINT			getWidth();
		UINT			getHeight();

		void			setX(INT iValue)	{ m_iX = iValue; }
		INT				getX()	const		{ return m_iX; }
		void			setY(INT iValue)	{ m_iY = iValue; }	
		INT				getY()	const		{ return m_iY; }			

		void			setScaleWidth(UINT uiValue)		{ m_uiScaleWidth = uiValue; }
		UINT			getScaleWidth() const			{ return m_uiScaleWidth; }
		void			setScaleHeight(UINT uiValue)	{ m_uiScaleHeight = uiValue; }
		UINT			getScaleHeight() const			{ return m_uiScaleHeight; }

		void			setVisible(BOOLEAN	bValue)		{ m_bVisible = bValue; }
		BOOLEAN			getVisible() const				{ return m_bVisible; }
		
		BOOLEAN			hitTest(INT iX, INT iY);
		BOOLEAN			isEmpty();


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Add an image into this movie clip.
		*
		* @param	strFileName	The image file path.
		*/
		void			addImage(const String &strFileName);

		void			addImage(const tagBitmapInfo& bitmap);
		/**
		* @brief	Get the current showing image.
		*
		& @return	Return the image file path.
		*/
		const String&	getCurrentImage();

		BOOLEAN getBitmap(String& strID, Gdiplus::Bitmap** pBitmap);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Get total number of images
		*/
		UINT			getTotalFrames() const;

		/**
		* Play all the images in this movie clip with a interval of uiIntervalTime in the order of add.
		*/
		void			play(UINT uiIntervalTime);
		void			stop();

		/**
		* @brief	Play from the image specified by index in this movie clip with a interval of uiIntervalTime in the order of add.
		*
		* @param	uiIndex It starts from 1.
		* @param	uiIntervalTime The interval time.
		*/
		void			gotoAndPlay(UINT uiIndex, UINT uiIntervalTime);

		/**
		* @brief	Show the image specified by index in this movie clip.
		*
		* @param	uiIndex It starts from 1.
		*/
		void			gotoAndStop(UINT uiIndex);

		/**
		* Play next image.
		*/
		void			nextFrame();

		/**
		* Play previous image.
		*/
		void			prevFrame();

		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);

		eImageType			getImageType() const {return m_eType;}
	protected:// method
		CMovieClip(const CMovieClip&){}
		CMovieClip& operator =(const CMovieClip&){return *this;}

	private:// method

		CSize			getCurrentImageSize();

	protected:// property

	private:// property
		eImageType		m_eType;
		Array2<String>	m_aryImage;
		UINT			m_uiCurrent;
		Array2<CSize>	m_arySize;
		Array2<tagBitmapInfo>	m_aryImageBitmap;

		INT				m_iX;
		INT				m_iY;
		UINT			m_uiScaleWidth;
		UINT			m_uiScaleHeight;
		BOOLEAN			m_bVisible;

		CTimer			m_timer;
	};
}