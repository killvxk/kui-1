#include "stdafx.h"

START_NAMESPACE_KCORE

struct TCursorOrIcon
{   
	WORD   Reserved;
	WORD   wType;
	WORD   Count;
};

struct TIconRec
{   
	BYTE   Width;
	BYTE   Height;
	WORD   Colors;
	WORD   Reserved1;
	WORD   Reserved2;
	long   DIBSize;
	long   DIBOffset;
};   

static void _InitBmpInfoHeader_(HBITMAP Bitmap, BITMAPINFOHEADER& BI, int nBit)   
{   
	int Bytes		= 0;
	DIBSECTION DS	= {0};
	DS.dsBmih.biSize= 0;

	Bytes = ::GetObject(Bitmap, sizeof(DS), &DS);
	if ( Bytes >= sizeof(DS.dsBm) + sizeof(DS.dsBmih) && 
		DS.dsBmih.biSize >= sizeof(DS.dsBmih))
	{
		::memcpy(&BI, &DS.dsBmih, sizeof(BITMAPINFOHEADER));
	}
	else
	{
		::memset(&BI, 0, sizeof(BI));
		BI.biSize = sizeof(BI);
		BI.biWidth = DS.dsBm.bmWidth;
		BI.biHeight = DS.dsBm.bmHeight;
		BI.biBitCount = DS.dsBm.bmPlanes * DS.dsBm.bmBitsPixel;
	}   

	if( nBit != 0 )
		BI.biBitCount = nBit;
	if( BI.biBitCount <= 8 )
		BI.biClrUsed = 1 << BI.biBitCount;

	BI.biPlanes = 1;

	if( BI.biClrImportant > BI.biClrUsed )
		BI.biClrImportant = BI.biClrUsed;
	if( BI.biSizeImage == 0 )
		BI.biSizeImage = ((BI.biWidth * BI.biBitCount + 31) / 32) * 4 * BI.biHeight;
}

static void _GetDIBSizes_(HBITMAP Bitmap, DWORD& InfoSize, DWORD& ImageSize, int nBit)   
{   
	BITMAPINFOHEADER BI = {0};

	_InitBmpInfoHeader_(Bitmap, BI, nBit);
	InfoSize = sizeof(BITMAPINFOHEADER);

	if( BI.biBitCount > 8 )
	{
		if  ( BI.biCompression & BI_BITFIELDS)
			InfoSize += 12;
	}
	else
		InfoSize += sizeof(RGBQUAD) * (BI.biClrUsed !=0 ? BI.biClrUsed: (1 << BI.biBitCount));

	ImageSize = BI.biSizeImage;   
}

static void _GetDIB_(HBITMAP Bitmap, BITMAPINFO* BmpInfo, void* Bits, int nBit)
{   
	HDC dc = ::CreateCompatibleDC(NULL);

	_InitBmpInfoHeader_(Bitmap, BmpInfo->bmiHeader, nBit);
	GetDIBits(dc, Bitmap, 0, BmpInfo->bmiHeader.biHeight, Bits, BmpInfo, DIB_RGB_COLORS);

	::DeleteDC(dc);
}

DWORD __stdcall AfxSaveHICONToData(HICON hIcon, LPCSTR data, DWORD length)
{
	if( hIcon == NULL )
		return 0;

	BYTE* pData = (BYTE*)data;
	ICONINFO   IconInfo = {0};
	IconInfo.fIcon = TRUE;
	::GetIconInfo(hIcon,   &IconInfo);

	BITMAPINFO *MonoInfo	= 0;
	void *MonoBits			= 0;
	DWORD MonoInfoSize		= 0, MonoBitsSize = 0;

	BITMAPINFO *ColorInfo	= 0;
	void *ColorBits			= 0;
	DWORD ColorInfoSize		= 0, ColorBitsSize = 0;

	__try
	{
		if( IconInfo.hbmMask )
		{
			_GetDIBSizes_(IconInfo.hbmMask, MonoInfoSize, MonoBitsSize, 1);
			MonoInfo = (BITMAPINFO*)new char[MonoInfoSize];
			MonoBits = new char [MonoBitsSize];
			_GetDIB_(IconInfo.hbmMask, MonoInfo, MonoBits, 1);
		}
		else
			return 0;

		if( IconInfo.hbmColor)
		{
			_GetDIBSizes_(IconInfo.hbmColor,   ColorInfoSize,   ColorBitsSize, 32);
			ColorInfo	=   (BITMAPINFO*)new char[ColorInfoSize];
			ColorBits   =   new char [ColorBitsSize];
			_GetDIB_(IconInfo.hbmColor,   ColorInfo,   ColorBits,   32);
		}
		else
			return 0;

		TCursorOrIcon CI = {0};
		CI.wType	= 0x0001;
		CI.Count	= 1;
		::memcpy(pData, &CI, sizeof(CI));
		pData += sizeof(CI);
		TIconRec   List = {0};
		List.Width   =   (BYTE)MonoInfo->bmiHeader.biWidth;
		List.Height  =   (BYTE)MonoInfo->bmiHeader.biHeight;
		if (ColorInfo)
		{
			List.Colors  =   ColorInfo->bmiHeader.biPlanes * ColorInfo->bmiHeader.biBitCount;
			List.DIBSize =   ColorInfoSize + ColorBitsSize + MonoBitsSize;
		} 
		else
		{
			List.Colors  = MonoInfo->bmiHeader.biPlanes * MonoInfo->bmiHeader.biBitCount;
			List.DIBSize = MonoInfoSize + MonoBitsSize;
		}
		List.DIBOffset = sizeof(CI) + sizeof(List);

		::memcpy(pData, &List, sizeof(List));
		pData += sizeof(List);

		if (ColorInfo)
		{
			ColorInfo->bmiHeader.biHeight   *=   2;
			::memcpy(pData, ColorInfo, ColorInfoSize);
			pData += ColorInfoSize;
			::memcpy(pData, ColorBits, ColorBitsSize);
			pData += ColorBitsSize;
		}
		else 
		{
			::memcpy(pData, MonoInfo, MonoInfoSize);
			pData += MonoInfoSize;
		}

		::memcpy(pData, MonoBits, MonoBitsSize);
		pData += MonoBitsSize;

		if (IconInfo.hbmMask)
			::DeleteObject(IconInfo.hbmMask);
		if (IconInfo.hbmColor)
			::DeleteObject(IconInfo.hbmColor);

	}
	__finally
	{
		if( ColorInfo != NULL )
			delete []ColorInfo;

		if( MonoInfo != NULL )
			delete []MonoInfo;

		if( ColorBits != NULL )
			delete []ColorBits;

		if( MonoBits != NULL )
			delete []MonoBits;
	}
	return (DWORD)(pData - (BYTE*)data);
}

HICON __stdcall	AfxLoadHICONFromData(LPCSTR data, DWORD length)
{
	if( length == 0 || data == NULL )
		return NULL;

	HICON hIcon				= NULL;	 
	TCHAR path[MAX_PATH]	= {0};
	HANDLE hFile			= INVALID_HANDLE_VALUE;
	DWORD dwWriteBytes		= 0;

	__try
	{
		::GetSystemDirectory(path, MAX_PATH);
		lstrcat(path, TEXT("\\pkg.ico"));
		::DeleteFile(path);

		hFile = ::CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
			__leave;

		::WriteFile(hFile, data, length, &dwWriteBytes, NULL);
		if( dwWriteBytes != length )
			__leave;
	}
	__finally
	{
		if( hFile != INVALID_HANDLE_VALUE )
			::CloseHandle(hFile);
	}

	if( hFile == INVALID_HANDLE_VALUE || dwWriteBytes != length )
		return NULL;

	return (HICON)::LoadImage(NULL, path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
}

END_NAMESPACE_KCORE