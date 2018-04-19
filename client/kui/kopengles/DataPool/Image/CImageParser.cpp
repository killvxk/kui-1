#include "ResourceSystem/ZipSkin.h"
#include "RenderEngine.h" 
using namespace Gdiplus;

namespace OpenHMI
{
	struct tagPixel
	{
		BYTE	btR;
		BYTE	btG;
		BYTE	btB;
		BYTE	btA;
	};

	CImageParser::tagImageInfo CImageParser::decodeFromFile(const String &strFilePath)
	{
		{
			CImageParser::tagImageInfo imgInfo;

			Bitmap *pBmp = Bitmap::FromFile((const CHAR*)strFilePath);

			if (pBmp == NULL || pBmp->GetWidth() == 0 || pBmp->GetHeight() == 0)
			{
				delete pBmp;

				pBmp = CZipSkin::getInstance()->LoadImageFromZip(strFilePath);
				if (pBmp == NULL || pBmp->GetWidth() == 0 || pBmp->GetHeight() == 0)
				{
					delete pBmp;
					return imgInfo;
				}
			}

			BitmapData bmpData;  
			Rect rect(0, 0, pBmp->GetWidth(), pBmp->GetHeight());  

			pBmp->LockBits(  
				&rect,  
				ImageLockModeRead,  
				PixelFormat32bppPARGB,
				&bmpData);

			// Write to the temporary buffer provided by LockBits.  
			UINT* pixels = (UINT*)bmpData.Scan0;  

			int byteCount = bmpData.Stride * bmpData.Height;  
			imgInfo.pbtPixelData = new BYTE[byteCount];

			if(imgInfo.pbtPixelData == NULL)
			{
				delete pBmp;
				return imgInfo;
			}
			memcpy(imgInfo.pbtPixelData, pixels, byteCount);  

			BYTE *pbtDestLine = imgInfo.pbtPixelData ;

			tagPixel *pPixelDest = NULL;

			for (UINT i =  0; i < pBmp->GetHeight(); i++)
			{
				pPixelDest = (tagPixel *)pbtDestLine;
				for (UINT j =  0; j < pBmp->GetWidth(); j++)
				{
					BYTE tmp = pPixelDest->btR;
					pPixelDest->btR = pPixelDest->btB;
					pPixelDest->btB = tmp;

					++pPixelDest;
				}

				pbtDestLine += pBmp->GetWidth() *4;
			}

			imgInfo.uiWidth      = pBmp->GetWidth();  
			imgInfo.uiHeight     = pBmp->GetHeight();

			pBmp->UnlockBits(&bmpData);
			delete pBmp;

			return imgInfo;
		}
	}

	CImageParser::tagImageInfo CImageParser::decodeFromBitmap(Gdiplus::Bitmap* pBitmap)
	{
		Bitmap *pBmp = pBitmap;
		CImageParser::tagImageInfo imgInfo;

		if (pBmp == NULL || pBmp->GetWidth() == 0 || pBmp->GetHeight() == 0)
		{
			CLog::warning2(RENDER_ENGINE_NAME, L"Can not open image file\n");
			return imgInfo;
		}

		BitmapData bmpData;  
		Rect rect(0, 0, pBmp->GetWidth(), pBmp->GetHeight());  

		pBmp->LockBits(  
			&rect,  
			ImageLockModeRead,  
			PixelFormat32bppPARGB,  
			&bmpData); 

		// Write to the temporary buffer provided by LockBits.  
		UINT* pixels = (UINT*)bmpData.Scan0;  

		int byteCount = bmpData.Stride * bmpData.Height;  
		imgInfo.pbtPixelData = new BYTE[byteCount];

		memcpy(imgInfo.pbtPixelData, pixels, byteCount);  

		BYTE *pbtDestLine = imgInfo.pbtPixelData ;

		tagPixel *pPixelDest = NULL;

		for (UINT i =  0; i < pBmp->GetHeight(); i++)
		{
			pPixelDest = (tagPixel *)pbtDestLine;
			for (UINT j =  0; j < pBmp->GetWidth(); j++)
			{
				BYTE tmp = pPixelDest->btR;
				pPixelDest->btR = pPixelDest->btB;
				pPixelDest->btB = tmp;

				++pPixelDest;
			}

			pbtDestLine += pBmp->GetWidth() *4;
		}

		imgInfo.uiWidth      = pBmp->GetWidth();  
		imgInfo.uiHeight     = pBmp->GetHeight();

		pBmp->UnlockBits(&bmpData);

		return imgInfo;
	}
}
