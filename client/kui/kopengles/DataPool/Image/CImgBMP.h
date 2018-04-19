#pragma once

namespace OpenHMI
{
	class CImgBMP
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

#pragma pack(push, bfhead) 
#pragma pack(1) 
		struct tagBitMapFileHeader
		{   
			USHORT usType;
			ULONG  ulSize;
			USHORT usReserved1;
			USHORT usReserved2;
			ULONG  ulOffBits;
		}; 

		struct tagBitMapInfoHeader
		{  
			ULONG  usSize;
			LONG   lWidth;
			LONG   lHeight;
			USHORT usPlanes;
			USHORT usBitCount;
			ULONG  blCompression;
			ULONG  blSizeImage;
			LONG   lXPelsPerMeter;
			LONG   lYPelsPerMeter;
			ULONG  lClrUsed;
			ULONG  lClrImportant;  
		}; 
#pragma pack(pop, bfhead)

	public:// method		
		static	CImgBMP*	getInstance();
		static	void		deleteInstance();

		CImageParser::tagImageInfo decodeFromFile(const String& strFileName);

	protected:// method
		CImgBMP();
		virtual ~CImgBMP();

	private:// method

	protected:// property

	private:// property
		static	CImgBMP	*s_pInstance;
	};
}