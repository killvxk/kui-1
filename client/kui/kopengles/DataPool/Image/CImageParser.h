#pragma once

namespace OpenHMI
{
	class CImageParser
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		class tagImageInfo
		{
		public:
			tagImageInfo()
				: uiWidth(0)
				, uiHeight(0)
				, pbtPixelData(NULL)
			{
				;
			}

			UINT	uiWidth;
			UINT	uiHeight;
			BYTE	*pbtPixelData;
		};

	private:// embed class or struct or enum

	public:// method	
		static tagImageInfo decodeFromFile(const String &strFilePath);
		static tagImageInfo decodeFromBitmap(Gdiplus::Bitmap* pBitmap);

	protected:// method
		CImageParser(){}
		CImageParser(const CImageParser&){}
		virtual ~CImageParser(){}		
		CImageParser& operator =(const CImageParser&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}