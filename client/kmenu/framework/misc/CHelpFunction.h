#pragma once

#include "OpenHMI.h"
#include <vector>
#include "giflib/gif_lib.h"

namespace OpenHMI
{

	struct GifImage
	{
		Gdiplus::Bitmap* image;
		int delay;
	};

	struct FileData
	{
		const unsigned char*	data;
		int size;
		int pos;
	};

	bool LoadGif(std::vector<Gdiplus::Bitmap*>* gif_image, int* gif_delay, const unsigned char* data_buffer, unsigned long size);

	struct rgb_color { unsigned char r, g, b; };

	class PaletteImage
	{
	public:
		PaletteImage(int width, int height)
		{
			m_width = width;
			m_height = height;
			m_bits = (unsigned char*)malloc(width*height);
			m_delay = 0;
			m_trans_color = -1;
		}

		~PaletteImage()
		{
			if (m_bits)
			{
				free(m_bits);
			}
		}

		rgb_color	m_pal[256];
		unsigned char*	m_bits;
		int	m_width;
		int m_height;
		int m_trans_color;
		int m_delay;
	};

	bool LoadGif(std::vector<GifImage>* gif_image, const unsigned char* data_buffer, unsigned long size);

	class CHelpFunction
	{
	public:
		static BOOLEAN 	getFileNameNotExtern(const String& strFileName, String& strFileTitle);
	};
}