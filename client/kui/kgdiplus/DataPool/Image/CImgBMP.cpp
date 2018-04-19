#include "RenderEngine.h"
#include "CImgBMP.h"
namespace OpenHMI
{
	CImgBMP	*CImgBMP::s_pInstance = NULL;

	CImgBMP* CImgBMP::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CImgBMP();
		}

		return s_pInstance;
	}

	void CImgBMP::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CImgBMP::CImgBMP()
	{

	}

	CImgBMP::~CImgBMP()
	{

	}

	CImageParser::tagImageInfo CImgBMP::decodeFromFile(const String& strFilePath)
	{
		CImageParser::tagImageInfo imgInfo;

		CFile file;
		if (FALSE == file.open(strFilePath, TRUE))
		{
			return imgInfo;
		}

		tagBitMapFileHeader bitMapFileHeader;
		file.readData((BYTE*)&bitMapFileHeader, sizeof(tagBitMapFileHeader));

		// the file isn't bmp
		if (bitMapFileHeader.usType != 0x4D42)
		{
			file.close();
			return imgInfo;
		}

		tagBitMapInfoHeader bitBitMapInfoHeader;
		file.readData((BYTE*)&bitBitMapInfoHeader, sizeof(tagBitMapInfoHeader));
		
		if (   bitBitMapInfoHeader.lWidth== 0 
			|| bitBitMapInfoHeader.lHeight == 0 
			|| (bitBitMapInfoHeader.usBitCount != 24 && bitBitMapInfoHeader.usBitCount != 32))
		{
			file.close();
			return imgInfo;
		}

		imgInfo.uiWidth      = (UINT)bitBitMapInfoHeader.lWidth;  
		imgInfo.uiHeight     = (UINT)Math::abs((INT)bitBitMapInfoHeader.lHeight);
		imgInfo.pbtPixelData = new BYTE[imgInfo.uiWidth * imgInfo.uiHeight * 4];
		
		BYTE*   pbtPixelDstCur  = imgInfo.pbtPixelData;
		BYTE*   pbtPixelSrcCur  = NULL;
		
		INT iStride	     = (imgInfo.uiWidth * bitBitMapInfoHeader.usBitCount + 31) / 32 * 4;
		BYTE * pbtSrcDIB = new BYTE[iStride * imgInfo.uiHeight];
		file.readData(pbtSrcDIB, iStride * imgInfo.uiHeight);

		if (bitBitMapInfoHeader.usBitCount == 24)
		{ 
			for (UINT i = 0; i < imgInfo.uiHeight; i++)  
			{ 
				if (bitBitMapInfoHeader.lHeight > 0)
				{
					pbtPixelSrcCur = pbtSrcDIB + iStride * (imgInfo.uiHeight - 1 - i);
				}
				else
				{
					pbtPixelSrcCur = pbtSrcDIB + iStride * i;
				}
				
				for (UINT j = 0; j < imgInfo.uiWidth; j++)  
				{  
					*pbtPixelDstCur++ = pbtPixelSrcCur[0];	// blue
					*pbtPixelDstCur++ = pbtPixelSrcCur[1];	// green
					*pbtPixelDstCur++ = pbtPixelSrcCur[2];	// red
					*pbtPixelDstCur++ = 255;				// alpha
					
					pbtPixelSrcCur += 3;
				}
			}

			
		}
		else if (bitBitMapInfoHeader.usBitCount == 32)
		{
			INT uiAlpha255To256 = 256;
			for (UINT i = 0; i < imgInfo.uiHeight; i++)  
			{ 
				if (bitBitMapInfoHeader.lHeight > 0)
				{
					pbtPixelSrcCur = pbtSrcDIB + iStride * (imgInfo.uiHeight - 1 - i);
				}
				else
				{
					pbtPixelSrcCur = pbtSrcDIB + iStride * i;
				}

				uiAlpha255To256 = ALPHA_255_TO_256(pbtPixelSrcCur[3]);
				
				for (UINT j = 0; j < imgInfo.uiWidth; j++)  
				{  
					uiAlpha255To256 = ALPHA_255_TO_256(pbtPixelSrcCur[3]);

					*pbtPixelDstCur++ = (pbtPixelSrcCur[0] * uiAlpha255To256) >> 8;	// blue
					*pbtPixelDstCur++ = (pbtPixelSrcCur[1] * uiAlpha255To256) >> 8;	// green
					*pbtPixelDstCur++ = (pbtPixelSrcCur[2] * uiAlpha255To256) >> 8;	// red
					*pbtPixelDstCur++ = pbtPixelSrcCur[3];							// alpha

					pbtPixelSrcCur += 4;
				}
			}
		}

		delete[] pbtSrcDIB;
		
		return imgInfo;  
	}
}

