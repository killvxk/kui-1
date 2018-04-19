#include "HMI.h"

namespace OpenHMI
{
	static int InterlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
	static int InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */

#define GIF_CONTROL_EXT_CODE 0xF9
#define GIF_CONTROL_EXT_SIZE 4

	static int readFunc(GifFileType* GifFile, GifByteType* buf, int count)
	{
		FileData* ptr = (FileData*)GifFile->UserData;
		int len = count;
		if (ptr->pos + len > ptr->size)
		{
			len = ptr->size - ptr->pos;
		}
		if (len > 0)
		{
			memcpy(buf, ptr->data + ptr->pos, len);
			ptr->pos += len;
		}
		return len;
	}

	// TODO: implement static method here
	bool LoadGif(std::vector<Gdiplus::Bitmap*>* gif_image, int* gif_delay, const unsigned char* data_buffer, unsigned long size)
	{
		vector<GifImage> image;
		if (!LoadGif(&image, data_buffer, size))
		{
			return false;
		}
		*gif_delay = 0;
		for (int i = 0; i < (float)image.size(); i += 1)
		{
			gif_image->push_back(image[i].image);
			if (*gif_delay < image[i].delay)
			{
				*gif_delay = image[i].delay;
			}
		}
		return true;
	}

	bool LoadGif(vector<GifImage>* gif_image, const unsigned char* data_buffer, unsigned long size)
	{
		int	i, j, Row, Col, Width, Height, ExtCode, Count;
		GifRecordType RecordType;
		GifByteType *Extension;
		GifFileType *GifFile;
		ColorMapObject *ColorMap;

		int trans_color = -1;
		int delay = 0;

		FileData fData;
		fData.data = data_buffer;
		fData.size = size;
		fData.pos = 0;
		int error = 0;
		if ((GifFile = DGifOpen(&fData, readFunc, &error)) == NULL) {
			return false;
		}

		std::vector<PaletteImage*> vImage;
		PaletteImage* image = NULL;

		/* Scan the content of the GIF file and load the image(s) in: */
		do {
			if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
				goto failed;
			}
			switch (RecordType) {
			case IMAGE_DESC_RECORD_TYPE:
				if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
					goto failed;
				}
				Row = GifFile->Image.Top; /* Image Position relative to Screen. */
				Col = GifFile->Image.Left;
				Width = GifFile->Image.Width;
				Height = GifFile->Image.Height;

				// Update Color map
				ColorMap = (GifFile->Image.ColorMap
					? GifFile->Image.ColorMap
					: GifFile->SColorMap);

				image = new PaletteImage(GifFile->SWidth, GifFile->SHeight);
				image->m_trans_color = trans_color;
				image->m_delay = delay;
				vImage.push_back(image);
				i = ColorMap->ColorCount;
				if (i > 256)
				{
					goto failed;
				}
				while (--i >= 0)
				{
					GifColorType* color = &ColorMap->Colors[i];
					rgb_color& c = image->m_pal[i];
					c.r = color->Red;
					c.g = color->Green;
					c.b = color->Blue;
				}

				// set background
				memset(image->m_bits, GifFile->SBackGroundColor, image->m_width * image->m_height);

				if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
					GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
					goto failed;
				}
				if (GifFile->Image.Interlace) {
					/* Need to perform 4 passes on the images: */
					for (Count = i = 0; i < 4; i++)
						for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i])
						{
							if (DGifGetLine(GifFile, image->m_bits + image->m_width * j + Col, //&ScreenBuff[j][Col],
								Width) == GIF_ERROR)
							{
								goto failed;
							}
						}
				}
				else {
					for (i = 0; i < Height; i++)
					{
						if (DGifGetLine(GifFile, image->m_bits + image->m_width * Row + Col,//&ScreenBuff[Row++][Col],
							Width) == GIF_ERROR) {
							goto failed;
						}
						++Row;
					}
				}
				break;
			case EXTENSION_RECORD_TYPE:
				/* Skip any extension blocks in file: */
				if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
					goto failed;
				}
				else
				{
					if (ExtCode == GIF_CONTROL_EXT_CODE && Extension)
					{
						if (Extension[0] == GIF_CONTROL_EXT_SIZE)
						{
							delay = (Extension[3] << 8 | Extension[2]) * 10;
						}
						if ((Extension[1] & 1) == 1)
						{
							trans_color = Extension[4];
						}
						else
						{
							trans_color = -1;
						}
					}
				}

				while (Extension != NULL) {
					if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
						goto failed;
					}
					else
					{
						if (ExtCode == GIF_CONTROL_EXT_CODE && Extension)
						{
							if (Extension[0] == GIF_CONTROL_EXT_SIZE)
							{
								delay = (Extension[3] << 8 | Extension[2]) * 10;
							}
							if ((Extension[1] & 1) == 1)
							{
								trans_color = Extension[4];
							}
							else
							{
								trans_color = -1;
							}
						}
					}
				}
				break;
			case TERMINATE_RECORD_TYPE:
				break;
			default:		    /* Should be traps by DGifGetRecordType. */
				break;
			}
		} while (RecordType != TERMINATE_RECORD_TYPE);

		/* Close file when done */
		error = 0;
		if (DGifCloseFile(GifFile, &error) == GIF_ERROR) {
			goto failed;
		}

		if (vImage.size() == 0)
		{
			return false;
		}

		for (int i = 0; i < (float)gif_image->size(); ++i)
		{
			delete (*gif_image)[i].image;
		}
		gif_image->clear();

		unsigned char* buffer = (unsigned char*)malloc(Width * Height * 4);

		for (size_t ii = 0; ii < vImage.size(); ++ii)
		{
			PaletteImage* image = vImage[ii];

			Gdiplus::Bitmap* image2 = new Gdiplus::Bitmap(Width, Height, PixelFormat32bppARGB);

			unsigned char* dst_bits = buffer;
			unsigned long dst_pitch = Width * 4;

			for (int y = 0; y < image->m_height; ++y)
			{
				unsigned char* p1 = dst_bits + y * dst_pitch;
				unsigned char* p2 = image->m_bits + y * image->m_width;
				for (int x = 0; x < image->m_width; ++x, p1 += 4, ++p2)
				{
					if (*p2 == image->m_trans_color)
					{
						p1[0] = 255;
						p1[1] = 255;
						p1[2] = 255;
						p1[3] = 0;
					}
					else
					{
						rgb_color& c = image->m_pal[*p2];
						p1[0] = c.b;
						p1[1] = c.g;
						p1[2] = c.r;
						p1[3] = 255;
					}
				}
			}

		{
			Gdiplus::BitmapData data2;
			Gdiplus::Rect rc(0, 0, Width, Height);
			data2.Scan0 = (void*)buffer;
			data2.Stride = 4 * Width;
			if (Gdiplus::Ok == image2->LockBits(&rc,
				Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
				PixelFormat32bppARGB, &data2))
			{
				image2->UnlockBits(&data2);
			}
		}

		GifImage g;
		g.image = image2;
		g.delay = image->m_delay;

		gif_image->push_back(g);

		delete image;
		}

		free(buffer);

		return true;

	failed:

		for (size_t ii = 0; ii < vImage.size(); ++ii)
		{
			delete vImage[ii];
		}

		return false;
	}

	BOOLEAN CHelpFunction::getFileNameNotExtern(const String& strFileName, String& strFileTitle)
	{
#define FILE_EXTERN_SEPARATOR	L'.'

		INT iDot = strFileName.length() - 1;
		while (iDot >= 0)
		{
			if (strFileName.charAt((UINT)iDot) == FILE_EXTERN_SEPARATOR)
			{
				break;
			}
			else
			{
				iDot--;
			}
		}

		if (iDot <= 0)
		{
			return FALSE;
		}

		strFileTitle = strFileName.subString(0, (UINT)iDot);
		return TRUE;
	}
}