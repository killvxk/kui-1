#include "RenderEngine.h"

#include "CImgPNG.h"

namespace OpenHMI
{
	CImgPNG	*CImgPNG::s_pInstance = NULL;

	CImgPNG* CImgPNG::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CImgPNG();
		}

		return s_pInstance;
	}

	void CImgPNG::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CImgPNG::CImgPNG()
	{
#ifdef OS_WIN

#endif

#ifdef OS_QNX
		m_imglib = NULL;
#endif

#ifdef OS_LINUX

#endif
	}

	CImgPNG::~CImgPNG()
	{
#ifdef OS_WIN

#endif

#ifdef OS_QNX
		if (m_imglib != NULL)
		{
			img_lib_detach(m_imglib);
		}
#endif

#ifdef OS_LINUX

#endif
	}

#ifdef OS_WIN
	CImageParser::tagImageInfo CImgPNG::decodeFromFile(const String &strFilePath)
	{
		CImageParser::tagImageInfo imgInfo;	

		char acFilePath[1024] = {0};
		String::convertToMBCS(strFilePath, acFilePath, 1024);

		FILE *pFile = NULL;
		fopen_s(&pFile, acFilePath, "rb");
		if (pFile == NULL)
		{
			return imgInfo;
		}

		char png_header[8] = {0};
		fread(png_header, 8, 1, pFile);
		if (png_sig_cmp((png_bytep)png_header, 0, 8))
		{
			fclose(pFile);
			return imgInfo;
		}

		// initialize structures for reading a png file
		png_structp	png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_infop	png_info_ptr = png_create_info_struct(png_struct_ptr);
		if (png_info_ptr == NULL)
		{
			png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
			fclose(pFile);
			return imgInfo;
		}

		// I/O initialization methods
		png_init_io(png_struct_ptr, pFile);
		png_set_sig_bytes(png_struct_ptr, 8);

		// read and parser png data
		png_read_png(png_struct_ptr, png_info_ptr, PNG_TRANSFORM_EXPAND, 0);		
		if (png_info_ptr->width == 0 || png_info_ptr->height == 0)
		{
			png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
			fclose(pFile);

			return imgInfo;
		}

		// need to free -- NO, see http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
		png_bytep *png_byte_ptr_ptr = png_get_rows(png_struct_ptr, png_info_ptr);

		INT			iWidth = png_info_ptr->width;
		INT			iHeight = png_info_ptr->height;		
		BYTE		*pbtPixelData = new BYTE[iWidth * iHeight * 4];
		BYTE		*pbtPixelDataCur = pbtPixelData;	
		png_bytep	png_byte_ptr = NULL;
		UINT		uiAlpha255To256 = 256;		

		// Original PNG pixel data stored from top-left corner, BUT OGLES Texture drawing is from bottom-left corner
		if (png_info_ptr->pixel_depth == 32)
		{
			for (INT iRow = 0; iRow < iHeight; iRow++)
			{
				png_byte_ptr = png_byte_ptr_ptr[iRow];

				for (INT iCol = 0; iCol < iWidth; iCol++, png_byte_ptr += 4)
				{
 					uiAlpha255To256 = ALPHA_255_TO_256(png_byte_ptr[3]);
 
 					*pbtPixelDataCur++ = (png_byte_ptr[2] * uiAlpha255To256) >> 8;	// blue
 					*pbtPixelDataCur++ = (png_byte_ptr[1] * uiAlpha255To256) >> 8;	// green
 					*pbtPixelDataCur++ = (png_byte_ptr[0] * uiAlpha255To256) >> 8;	// red
 					*pbtPixelDataCur++ = png_byte_ptr[3];							// alpha

					//*pbtPixelDataCur++ = png_byte_ptr[2];	// blue
					//*pbtPixelDataCur++ = png_byte_ptr[1];	// green
					//*pbtPixelDataCur++ = png_byte_ptr[0];	// red
					//*pbtPixelDataCur++ = png_byte_ptr[3];	// alpha
				}
			}
		}
		else if (png_info_ptr->pixel_depth == 24)
		{
			for (INT iRow = 0; iRow < iHeight; iRow++)
			{
				png_byte_ptr = png_byte_ptr_ptr[iRow];

				for (INT iCol = 0; iCol < iWidth; iCol++, png_byte_ptr += 3)
				{
					*pbtPixelDataCur++ = png_byte_ptr[2];	// blue
					*pbtPixelDataCur++ = png_byte_ptr[1];	// green
					*pbtPixelDataCur++ = png_byte_ptr[0];	// red
					*pbtPixelDataCur++ = 255;				// alpha
				}
			}
		}
		else
		{
			png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
			fclose(pFile);

			return imgInfo;
		}

		imgInfo.uiWidth = iWidth;
		imgInfo.uiHeight = iHeight;
		imgInfo.pbtPixelData = pbtPixelData;

		// clean up after the read, and free any memory allocated
		png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
		fclose(pFile);

		return imgInfo;
	}
#endif

#ifdef OS_QNX
	CImageParser::tagImageInfo CImgPNG::decodeFromFile(const String &strFilePath)
	{
		CImageParser::tagImageInfo imgInfo;

		if (m_imglib == NULL)
		{
			INT iRet = img_lib_attach(&m_imglib);
			if ( iRet == IMG_ERR_CFG)
			{
				CLog::error2(RENDER_ENGINE_NAME, L"img_lib_attach error IMG_ERR_CFG");
				if( img_cfg_read (m_imglib,"./img.conf") != IMG_ERR_OK )
				{
					CLog::error2(RENDER_ENGINE_NAME, L"img_cfg_read error");
					return imgInfo;
				}
			}
			else if ( iRet != IMG_ERR_OK)
			{
				m_imglib = NULL;
				CLog::error2(RENDER_ENGINE_NAME, L"img_lib_attach error");
				return imgInfo;
			}
		}
		
		img_t img;
		memset(&img, 0, sizeof(img));

		char acFilePath[1024] = {0};
		String::convertToMBCS(strFilePath, acFilePath, 1024);

		if (img_load_file(m_imglib, acFilePath, NULL, &img) != IMG_ERR_OK) 
		{
			CLog::info(L"***************************************", L"img_load_file");
			return imgInfo;
		}

		if (img.w == 0 || img.h == 0)
		{
			CLog::info(L"***************************************", L"img.w == 0 || img.h == 0");
			return imgInfo;
		}

		if (img.flags & IMG_DIRECT)
		{
			INT			iWidth = img.w;
			INT			iHeight = img.h;		
			BYTE		*pbtPixelData = new BYTE[iWidth * iHeight * 4];
			BYTE		*pbtPixelDataCur = pbtPixelData;
			BYTE		*pbtSrcData = img.access.direct.data;
			BYTE		*pbtSrcDataCur = pbtSrcData;
			UINT		uiAlpha255To256 = 256;
			
			// Original PNG pixel data stored from top-left corner, BUT OGLES Texture drawing is from bottom-left corner
			
			if (img.access.direct.stride / img.w == 4)
			{
				for (INT iRow = 0; iRow < iHeight; iRow++)
				{
					pbtSrcDataCur = pbtSrcData + img.access.direct.stride * iRow;

					for (INT iCol = 0; iCol < iWidth; iCol++, pbtSrcDataCur += 4)
					{
						uiAlpha255To256 = ALPHA_255_TO_256(pbtSrcDataCur[3]);

						*pbtPixelDataCur++ = (pbtSrcDataCur[0] * uiAlpha255To256) >> 8;	// blue
						*pbtPixelDataCur++ = (pbtSrcDataCur[1] * uiAlpha255To256) >> 8;	// green
						*pbtPixelDataCur++ = (pbtSrcDataCur[2] * uiAlpha255To256) >> 8;	// red
						*pbtPixelDataCur++ = pbtSrcDataCur[3];							// alpha

						//*pbtPixelDataCur++ = pbtSrcDataCur[0];	// blue
						//*pbtPixelDataCur++ = pbtSrcDataCur[1];	// green
						//*pbtPixelDataCur++ = pbtSrcDataCur[2];	// red
						//*pbtPixelDataCur++ = pbtSrcDataCur[3];	// alpha
					}
				}
				free(pbtSrcData);
			}
			else if (img.access.direct.stride / img.w == 3)
			{
				for (INT iRow = 0; iRow < iHeight; iRow++)
				{
					pbtSrcDataCur = pbtSrcData + img.access.direct.stride * iRow;

					for (INT iCol = 0; iCol < iWidth; iCol++, pbtSrcDataCur += 3)
					{
						*pbtPixelDataCur++ = pbtSrcDataCur[0];	// blue
						*pbtPixelDataCur++ = pbtSrcDataCur[1];	// green
						*pbtPixelDataCur++ = pbtSrcDataCur[2];	// red
						*pbtPixelDataCur++ = 255;				// alpha				
					}
				}
				free(pbtSrcData);
			}
			else
			{
				free(pbtSrcData);
				return imgInfo;
			}
			
			imgInfo.uiWidth = iWidth;
			imgInfo.uiHeight = iHeight;
			imgInfo.pbtPixelData = pbtPixelData;
		}

		return imgInfo;
	}
#endif

#ifdef OS_LINUX
	CImageParser::tagImageInfo CImgPNG::decodeFromFile(const String &strFilePath)
	{
		CImageParser::tagImageInfo imgInfo;

		char acFilePath[1024] = {0};
		String::convertToMBCS(strFilePath, acFilePath, 1024);

		FILE *pFile = fopen(acFilePath, "rb");
		if (pFile == NULL)
		{
			return imgInfo;
		}

		char png_header[8] = {0};
		fread(png_header, 8, 1, pFile);
		if (png_sig_cmp((png_bytep)png_header, 0, 8))
		{
			fclose(pFile);
			return imgInfo;
		}

		// initialize structures for reading a png file
		png_structp	png_struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_infop	png_info_ptr = png_create_info_struct(png_struct_ptr);
		if (png_info_ptr == NULL)
		{
			png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
			fclose(pFile);
			return imgInfo;
		}

		// I/O initialization methods
		png_init_io(png_struct_ptr, pFile);
		png_set_sig_bytes(png_struct_ptr, 8);

		// read and parser png data
		png_read_png(png_struct_ptr, png_info_ptr, PNG_TRANSFORM_EXPAND, 0);		
		if (png_info_ptr->width == 0 || png_info_ptr->height == 0)
		{
			png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
			fclose(pFile);

			return imgInfo;
		}

		// need to free -- NO, see http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
		png_bytep *png_byte_ptr_ptr = png_get_rows(png_struct_ptr, png_info_ptr);

		INT			iWidth = png_info_ptr->width;
		INT			iHeight = png_info_ptr->height;		
		BYTE		*pbtPixelData = new BYTE[iWidth * iHeight * 4];
		BYTE		*pbtPixelDataCur = pbtPixelData;
		png_bytep	png_byte_ptr = NULL;
		UINT		uiAlpha255To256 = 256;

		// Original PNG pixel data stored from top-left corner, BUT OGLES Texture drawing is from bottom-left corner
		if (png_info_ptr->pixel_depth == 32)
		{
			for (INT iRow = 0; iRow < iHeight; iRow++)
			{
				png_byte_ptr = png_byte_ptr_ptr[iRow];

				for (INT iCol = 0; iCol < iWidth; iCol++, png_byte_ptr += 4)
				{
 					uiAlpha255To256 = ALPHA_255_TO_256(png_byte_ptr[3]);
 
 					*pbtPixelDataCur++ = (png_byte_ptr[2] * uiAlpha255To256) >> 8;	// blue
 					*pbtPixelDataCur++ = (png_byte_ptr[1] * uiAlpha255To256) >> 8;	// green
 					*pbtPixelDataCur++ = (png_byte_ptr[0] * uiAlpha255To256) >> 8;	// red
 					*pbtPixelDataCur++ = png_byte_ptr[3];							// alpha

					//*pbtPixelDataCur++ = png_byte_ptr[0];	// blue
					//*pbtPixelDataCur++ = png_byte_ptr[1];	// green
					//*pbtPixelDataCur++ = png_byte_ptr[2];	// red
					//*pbtPixelDataCur++ = png_byte_ptr[3];	// alpha
				}
			}
		}
		else if (png_info_ptr->pixel_depth == 24)
		{
			for (INT iRow = 0; iRow < iHeight; iRow++)
			{
				png_byte_ptr = png_byte_ptr_ptr[iRow];

				for (INT iCol = 0; iCol < iWidth; iCol++, png_byte_ptr += 3)
				{
					*pbtPixelDataCur++ = png_byte_ptr[2];	// blue
					*pbtPixelDataCur++ = png_byte_ptr[1];	// green
					*pbtPixelDataCur++ = png_byte_ptr[0];	// red
					*pbtPixelDataCur++ = 255;				// alpha
				}
			}
		}
		else
		{
			png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
			fclose(pFile);

			return imgInfo;
		}

		imgInfo.uiWidth = iWidth;
		imgInfo.uiHeight = iHeight;
		imgInfo.pbtPixelData = pbtPixelData;

		// clean up after the read, and free any memory allocated
		png_destroy_read_struct(&png_struct_ptr, &png_info_ptr, png_infopp_NULL);
		fclose(pFile);

		return imgInfo;
	}
#endif
}