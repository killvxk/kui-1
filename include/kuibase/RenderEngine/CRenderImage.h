/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"
#include <GdiPlus.h>

namespace OpenHMI
{
	class OPENHMI_API CRenderImage : public CRenderItem
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ELayout
		{
			L_UNKNOWN			= 999,
			L_NORMAL			= 0,
			L_STRETCH_CENTER	= 1,
			L_STRETCH_FILL		= 2,
		};

	private:// embed class or struct or enum		

	public:// method		
		CRenderImage();
		virtual ~CRenderImage();

		void						setFilePath(const String &strValue);
		const String&				getFilePath() const;

		void						setBitmap(const String& strID, Gdiplus::Bitmap* pBitmap);
		Gdiplus::Bitmap*			getBitmap() const {return m_pBitmap;}

		
		void						setLayout(ELayout eValue);
		CRenderImage::ELayout		getLayout() const						{ return m_eLayout; }

		void						setNormalLayoutClipOffsetX(INT iValue);
		INT							getNormalLayoutClipOffsetX() const		{ return m_iClipOffsetX; }

		void						setNormalLayoutClipOffsetY(INT iValue);
		INT							getNormalLayoutClipOffsetY() const		{ return m_iClipOffsetY; }

		void						setIsBackGround(BOOLEAN bValue)			{ m_bIsBackGround = bValue; }
		BOOLEAN						getIsBackGround() const					{ return m_bIsBackGround; }

		void						setIsBitmap(BOOLEAN bValue) {m_bIsBitmap = bValue;}
		BOOLEAN						getIsBitmap() const {return m_bIsBitmap;}

	protected:// method
		CRenderImage(const CRenderImage&){}
		CRenderImage& operator =(const CRenderImage&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		String			m_strFilePath;

		ELayout			m_eLayout;
		INT				m_iClipOffsetX;
		INT				m_iClipOffsetY;

		Gdiplus::Bitmap* m_pBitmap;
		BOOLEAN			m_bIsBackGround;
		BOOLEAN			m_bIsBitmap;
	};
}