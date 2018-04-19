
#include "stdafx.h"
#include "ZipSkin.h"
#include <Windows.h>
#include <vector>
#include <algorithm>

CZipSkinEx* CZipSkinEx::getInstance()
{
	static CZipSkinEx skin;
	return &skin;
}

Gdiplus::Image* AfxUIGetImageEx(LPCTSTR FileName)
{
	if (FileName == NULL || *FileName == 0)
	{
		return NULL;
	}
	Gdiplus::Image* pImage = NULL;
	{
		CString strFile;
		strFile += _T("skin/");
		strFile += FileName;
		pImage = CZipSkinEx::getInstance()->LoadImageFromZipEx(strFile);
		if (pImage == NULL)
		{
			pImage = CZipSkinEx::getInstance()->LoadImageFromFileEx(strFile);
		}
	}
	return pImage;
}

Gdiplus::Image* CombineImage(Gdiplus::Image* normal_image, Gdiplus::Image* hover_image, Gdiplus::Image* press_image, Gdiplus::Image* disable_image)
{
	if (normal_image == NULL)
	{
		return NULL;
	}
	int w = normal_image->GetWidth();
	int h = normal_image->GetHeight();

	if (normal_image == NULL || hover_image == NULL) return NULL;

	if (normal_image != NULL && hover_image == NULL && press_image == NULL && disable_image == NULL)
	{
		return normal_image;
	}

	if (w != hover_image->GetWidth() || h != hover_image->GetHeight()) return NULL;

	if (disable_image && press_image)
	{
		// 4
		if (w != disable_image->GetWidth() || h != disable_image->GetHeight()) return NULL;
		if (w != press_image->GetWidth() || h != press_image->GetHeight()) return NULL;

		Gdiplus::Image* image_ret = new Gdiplus::Bitmap(w*4,h, PixelFormat32bppARGB);

		{
			Gdiplus::Graphics g(image_ret);
			g.Clear(Gdiplus::Color(0,0,0,0));

			g.DrawImage(normal_image, Gdiplus::Rect(w*0, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
			g.DrawImage(hover_image, Gdiplus::Rect(w*1, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
			g.DrawImage(press_image, Gdiplus::Rect(w*2, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
			g.DrawImage(disable_image, Gdiplus::Rect(w*3, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
		}

		return image_ret;
	}
	else if (press_image)
	{
		// 3
		if (w != press_image->GetWidth() || h != press_image->GetHeight()) return NULL;

		Gdiplus::Image* image_ret = new Gdiplus::Bitmap(w*3,h, PixelFormat32bppARGB);

		{
			Gdiplus::Graphics g(image_ret);
			g.Clear(Gdiplus::Color(0,0,0,0));

			g.DrawImage(normal_image, Gdiplus::Rect(w*0, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
			g.DrawImage(hover_image, Gdiplus::Rect(w*1, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
			g.DrawImage(press_image, Gdiplus::Rect(w*2, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
		}

		return image_ret;
	}
	else
	{
		// 2
		Gdiplus::Image* image_ret = new Gdiplus::Bitmap(w*2,h, PixelFormat32bppARGB);

		{
			Gdiplus::Graphics g(image_ret);
			g.Clear(Gdiplus::Color(0,0,0,0));

			g.DrawImage(normal_image, Gdiplus::Rect(w*0, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
			g.DrawImage(hover_image, Gdiplus::Rect(w*1, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);
		}

		return image_ret;
	}
}

Gdiplus::Image* CombineImage(LPCTSTR normal_image, LPCTSTR hover_image, LPCTSTR press_image, LPCTSTR disable_image)
{
	Gdiplus::Image* image1 = AfxUIGetImageEx(normal_image);
	Gdiplus::Image* image2 = AfxUIGetImageEx(hover_image);
	Gdiplus::Image* image3 = AfxUIGetImageEx(press_image);
	Gdiplus::Image* image4 = AfxUIGetImageEx(disable_image);

	if (image1 != NULL && image2 == NULL && image3 == NULL && image4 == NULL)
	{
		return image1;
	}

	Gdiplus::Image* image_ret = CombineImage(image1, image2, image3, image4);

	if (image1) delete image1;
	if (image2) delete image2;
	if (image3) delete image3;
	if (image4) delete image4;

	return image_ret;
}

Gdiplus::Image* CombineImageEx(LPCTSTR image1, ...)
{
	va_list vargs;
	va_start(vargs, image1);

	Gdiplus::Image* m = AfxUIGetImageEx(image1);
	if (m == NULL) return NULL;

	std::vector<Gdiplus::Image*>	v;
	v.push_back(m);

	int w = m->GetWidth();
	int h = m->GetHeight();

	do 
	{
		LPCTSTR image_next = va_arg( vargs, LPCTSTR);
		if (image_next)
		{
			Gdiplus::Image* image = AfxUIGetImageEx(image_next);
			if (image == NULL)
			{
				goto load_error;
			}
			if (image->GetWidth() != w || image->GetHeight() != h)
			{
				goto load_error;
			}
			v.push_back(image);
		}
		else
		{
			break;
		}
	}
	while (true);

	va_end(vargs);

	Gdiplus::Image* image_ret = new Gdiplus::Bitmap(w * (int)v.size(),h, PixelFormat32bppARGB);

	{
		Gdiplus::Graphics g(image_ret);
		g.Clear(Gdiplus::Color(0,0,0,0));


		for (int i = 0; i < (int)v.size(); ++i)
		{
			Gdiplus::Image* image = v[i];
			g.DrawImage(image, Gdiplus::Rect(w * i, 0, w, h), 0, 0, w, h, Gdiplus::UnitPixel, NULL);

			delete image;
		}
	}

	return image_ret;

load_error:

	for (int i = 0; i < (int)v.size(); ++i)
	{
		Gdiplus::Image* image = v[i];
		delete image;
	}

	return NULL;
}


Gdiplus::Image* CombineImageResizeEx(int icon_width, int icon_height, LPCTSTR image1, ...)
{
	va_list vargs;
	va_start(vargs, image1);

	std::vector<Gdiplus::Image*>	v;

	LPCTSTR image_next = image1;
	do 
	{
		if (image_next)
		{
			Gdiplus::Image* image = AfxUIGetImageEx(image_next);
			if (image == NULL)
			{
				goto load_error;
			}
			if (image->GetWidth() != icon_width || image->GetHeight() != icon_height)
			{
				Gdiplus::Image* resize_image = new Gdiplus::Bitmap(icon_width, icon_height, PixelFormat32bppARGB);
				Gdiplus::Graphics g(resize_image);

				g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				g.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);

				g.DrawImage(image, Gdiplus::Rect(0,0,icon_width,icon_height), 0, 0, image->GetWidth(), image->GetHeight(), Gdiplus::UnitPixel, NULL);

				delete image;
				image = resize_image;
			}
			v.push_back(image);
			image_next = va_arg( vargs, LPCTSTR);
		}
		else
		{
			break;
		}
	}
	while (true);

	va_end(vargs);

	Gdiplus::Image* image_ret = new Gdiplus::Bitmap(icon_width * (int)v.size(),icon_height, PixelFormat32bppARGB);

	{
		Gdiplus::Graphics g(image_ret);
		g.Clear(Gdiplus::Color(0,0,0,0));


		for (int i = 0; i < (int)v.size(); ++i)
		{
			Gdiplus::Image* image = v[i];
			g.DrawImage(image, Gdiplus::Rect(icon_width * i, 0, icon_width, icon_height), 0, 0, icon_width, icon_height, Gdiplus::UnitPixel, NULL);

			delete image;
		}
	}

	return image_ret;

load_error:

	for (int i = 0; i < (int)v.size(); ++i)
	{
		Gdiplus::Image* image = v[i];
		delete image;
	}

	return NULL;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

bool SaveImage(const TCHAR* filename, Gdiplus::Image* image, int quality)
{
	const wchar_t* ext = wcsrchr(filename, L'.');
	wchar_t* type_name = NULL;
	if (wcscmp(ext,L".jpg") == 0)
	{
		type_name = L"image/jpeg";
	}
	else if (wcscmp(ext,L".bmp") == 0)
	{
		type_name = L"image/bmp";
	}
	else if (wcscmp(ext,L".gif") == 0)
	{
		type_name = L"image/gif";
	}
	else if (wcscmp(ext,L".png") == 0)
	{
		type_name = L"image/png";
	}
	else
	{
		return false;
	}

	CLSID clsid;
	GetEncoderClsid(type_name, &clsid);

	Gdiplus::EncoderParameters* pParams = NULL;
	Gdiplus::EncoderParameters param;
	if (wcscmp(type_name,L"image/jpeg") == 0)
	{
		param.Count = 1;
		param.Parameter[0].Guid = Gdiplus::EncoderQuality;
		param.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		param.Parameter[0].NumberOfValues = 1;
		param.Parameter[0].Value = &quality;
		pParams = &param;
	}
	Gdiplus::Status status = image->Save(filename, &clsid, pParams);
	if (status != Gdiplus::Ok)
	{
		return false;
	}
	return true;
}

typedef struct tagIconDirectoryEntry {
	BYTE  bWidth;
	BYTE  bHeight;
	BYTE  bColorCount;
	BYTE  bReserved;
	WORD  wPlanes;
	WORD  wBitCount;
	DWORD dwBytesInRes;
	DWORD dwImageOffset;
} ICONDIRENTRY;

typedef struct tagIconDir {
	WORD          idReserved;
	WORD          idType;
	WORD          idCount;
} ICONHEADER;

bool SortIconEntry(ICONDIRENTRY& icon1, ICONDIRENTRY& icon2)
{
	return icon1.bWidth < icon2.bWidth;
}

class ReadBuffer
{
public:
	ReadBuffer()
	{
		m_data = NULL;
		m_size = 0;
		m_pos = 0;
	}

	ReadBuffer(unsigned int size)
	{
		if (size == 0)
		{
			m_data = NULL;
			m_size = 0;
		}
		else
		{
			m_data = (unsigned char*)malloc(size);
			m_size = size;
		}
		m_pos = 0;
	}

	ReadBuffer(void* data, unsigned long size, bool is_copy = true)
	{
		if (is_copy)
		{
			m_size = size;
			m_data = (unsigned char*)malloc(m_size);
			memcpy(m_data, data, m_size);
		}
		else
		{
			m_data = (unsigned char*)data;
			m_size = size;
		}
		m_pos = 0;
	}

	virtual ~ReadBuffer()
	{
		if (m_data)
		{
			free(m_data);
			m_data = NULL;
		}
	}

	unsigned char* data()
	{
		return m_data;
	}

	unsigned int size()
	{
		return m_size;
	}

	unsigned char* offset(unsigned int offset)
	{
		if (offset > m_size || offset < 0 || m_data == NULL)
		{
			return NULL;
		}
		return m_data + offset;
	}

	void set_zero()
	{
		if (m_data)
		{
			memset(m_data, 0 , m_size);
		}
	}

	void assign(void* data, unsigned int size)
	{
		if (m_size > 0)
		{
			if (m_size >= size)
			{
				memcpy(m_data, data, size);
				m_size = size;
				return;
			}
			else
			{
				free(m_data);
				m_data = NULL;
				m_size = 0;
			}
		}
		m_size = size;
		m_data = (unsigned char*)malloc(m_size);
		memcpy(m_data, data, m_size);
	}

	void attach(void* data, unsigned int size)
	{
		if (m_size > 0)
		{
			if (data != m_data)
			{
				free(m_data);
				m_data = NULL;
				m_size = 0;
			}
		}
		m_size = size;
		m_data = (unsigned char*)data;
	}

	bool set_size(unsigned int size)
	{
		if (m_size == size)
		{
			return true;
		}

		if (size == 0)
		{
			if (m_data)
			{
				free(m_data);
				m_data = NULL;
			}
			m_size = 0;
			return true;
		}
		else if (size > m_size)
		{
			m_data = (unsigned char*)realloc(m_data, size);
			memset(m_data + m_size, 0, size - m_size);
			m_size = size;
			if (m_data == NULL)
			{
				m_size = 0;
			}
		}
		else if (m_size > 0)
		{
			m_size = size;
		}
		return m_data != NULL;
	}

	void raw_set_size(unsigned int size)
	{
		m_size = size;
	}

	bool set_pos(unsigned int pos)
	{
		if (pos >= 0 && pos <= m_size)
		{
			m_pos = pos;
			return true;
		}
		else
		{
			if (pos < 0) pos = 0;
			if (pos > m_size) pos = m_size;
			m_pos = pos;
			return false;
		}
	}

	unsigned int pos()
	{
		return m_pos;
	}

	bool read_byte(unsigned char& value)
	{
		if (m_pos + 1 <= m_size)
		{
			value = *((unsigned char*)(m_data+m_pos));
			m_pos += 1;
			return true;
		}
		return false;
	}

	bool read_int(int& value)
	{
		if (m_pos + 4 <= m_size)
		{
			value = *((int*)(m_data+m_pos));
			m_pos += 4;
			return true;
		}
		return false;
	}

	bool read_uint(unsigned int& value)
	{
		if (m_pos + 4 <= m_size)
		{
			value = *((unsigned int*)(m_data+m_pos));
			m_pos += 4;
			return true;
		}
		return false;
	}

	bool read_ushort(unsigned short& value)
	{
		if (m_pos + 2 <= m_size)
		{
			value = *((unsigned short*)(m_data+m_pos));
			m_pos += 2;
			return true;
		}
		return false;
	}

	bool read_short(short& value)
	{
		if (m_pos + 2 <= m_size)
		{
			value = *((short*)(m_data+m_pos));
			m_pos += 2;
			return true;
		}
		return false;
	}

	bool read_float(float& value)
	{
		if (m_pos + 4 <= m_size)
		{
			value = *((float*)(m_data+m_pos));
			m_pos += 4;
			return true;
		}
		return false;
	}

	bool read_long_long(long long& value)
	{
		if (m_pos + 8 <= m_size)
		{
			value = *((long long*)(m_data+m_pos));
			m_pos += 8;
			return true;
		}
		return false;
	}

	bool read_string(char* value, int len)
	{
		if (m_pos + len <= m_size)
		{
			memcpy(value,m_data+m_pos,len);
			value[len] = '\0';
			m_pos += len;
			return true;
		}
		return false;
	}

	bool read_string(std::string& value, int len)
	{
		if (m_pos + len <= m_size)
		{
			value.assign((const char*)(m_data+m_pos),len);
			m_pos += len;
			return true;
		}
		return false;
	}

	bool read_bytes(unsigned char* value, int len)
	{
		if (m_pos + len <= m_size)
		{
			memcpy(value,m_data+m_pos,len);
			//value[len] = '\0';
			m_pos += len;
			return true;
		}
		return false;
	}

	void clear()
	{
		if (m_data)
		{
			::free(m_data);
			m_data = NULL;
			m_size = 0;
			m_pos = 0;
		}
	}

	void detach()
	{
		m_data = NULL;
		m_size = 0;
		m_pos = 0;
	}

public:

	unsigned char*	m_data;
	unsigned long	m_size;
	unsigned int	m_pos;
};


Gdiplus::Image* ReadIcon(LPCTSTR iconfile, int icon_size)
{
	errno_t err = 0;
	FILE* fp = NULL;
	err = _wfopen_s(&fp,iconfile, _T("rb"));
	if (err != 0)
		return NULL;

	unsigned char* data = NULL;
	Gdiplus::Bitmap* image = NULL;
	std::vector<ICONDIRENTRY>	icon_entrys;

	ICONHEADER icon_header;
	int ret = 0;
	ret = (int)fread(&icon_header, sizeof(icon_header), 1, fp);
	if (ret != 1)
	{
		goto failed;
	}

	//icon_header.idType = icon_header.idType;
	//icon_header.idCount = icon_header.idCount;

	for (int i = 0; i < icon_header.idCount; ++i)
	{
		ICONDIRENTRY icon;
		ret = (int)fread(&icon, sizeof(icon), 1, fp);
		if (ret != 1)
		{
			goto failed;
		}

		icon_entrys.push_back(icon);
	}

	// 查找32位色的最佳匹配
	ICONDIRENTRY* best_icon = NULL;
	for (int i = 0; i < (int)icon_entrys.size(); ++i)
	{
		ICONDIRENTRY& icon = icon_entrys[i];
		if (icon.wBitCount == 32)
		{
			if (icon.bWidth == icon_size)
			{
				best_icon = &icon;
				break;
			}
		}
	}
	if (best_icon == NULL)
	{
		std::sort(icon_entrys.begin(),icon_entrys.end(), SortIconEntry);
		for (int i = 0; i < (int)icon_entrys.size(); ++i)
		{
			ICONDIRENTRY& icon = icon_entrys[i];
			if (icon.wBitCount == 32 || icon.bColorCount == 32)
			{
				if (icon.bWidth >= icon_size)
				{
					best_icon = &icon;
					break;
				}
			}
		}
	}

	if (best_icon == NULL)
	{
		fclose(fp);
		// 没有32位色的最佳匹配，就用Windows API加载
		HICON hIcon = NULL;
		//LoadIconWithScaleDown(NULL, iconfile, 48, 48, &hIcon);
		hIcon = reinterpret_cast<HICON>(::LoadImage(0, iconfile, IMAGE_ICON, 
			48, 48, LR_LOADFROMFILE|LR_DEFAULTCOLOR));
		if (hIcon)
		{
			Gdiplus::Bitmap* image = new Gdiplus::Bitmap(hIcon);
			DeleteObject(hIcon);
			return image;
		}
		return NULL;
	}

	ret = fseek(fp, best_icon->dwImageOffset, SEEK_SET);
	if (ret != 0)
	{
		goto failed;
	}

	// 读取头
	BITMAPINFOHEADER header;
	ret = (int)fread(&header, sizeof(header), 1, fp);
	if (ret != 1)
	{
		goto failed;
	}

	int width = best_icon->bWidth;
	int height = best_icon->bWidth;
	if (height < 0) height = -height;
	int data_size = 4 * width * height;
	data = (unsigned char*)malloc(data_size);
	if (data == NULL)
	{
		goto failed;
	}

	ret = (int)fread(data, data_size, 1, fp);
	if (ret != 1)
	{
		goto failed;
	}

	{
		image = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

		// 图像是反的，要调转一次
		unsigned char* buffer = (unsigned char*)malloc(data_size);
		if (buffer == NULL)
		{
			goto failed;
		}
		int pitch = 4 * width;
		for (int i = 0; i < height; ++i)
		{
			memcpy(buffer+(pitch*(height-i-1)), data+(pitch*i), pitch);
		}

		Gdiplus::BitmapData data2;
		Gdiplus::Rect rc(0,0,width,height);
		data2.Scan0 = (void*)buffer;
		data2.Stride = 4 * width;
		if (Gdiplus::Ok == image->LockBits(&rc, 
			Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf, 
			PixelFormat32bppARGB, &data2))
		{
			image->UnlockBits(&data2);
		}
		free(buffer);
	}

	fclose(fp);
	fp = NULL;

	if (data)
	{
		free(data);
		data = NULL;
	}

	if (width == icon_size && height == icon_size)
	{
		return image;
	}

	{
		Gdiplus::Bitmap* image2 = new Gdiplus::Bitmap(icon_size, icon_size, PixelFormat32bppARGB);
		Gdiplus::Graphics g(image2);

		g.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
		g.DrawImage(image, Gdiplus::Rect(0,0,icon_size,icon_size), 0,0,width,height, Gdiplus::UnitPixel);

		delete image;
		return image2;
	}

failed:

	if (fp)
	{
		fclose(fp);
	}
	if (data) free(data);
	return NULL;
}


Gdiplus::Image* ReadIcon(void* icon_data, unsigned int icon_data_size, int icon_size)
{
	ReadBuffer read_buffer((unsigned char*)icon_data, icon_data_size, false);

	unsigned char* data = NULL;
	Gdiplus::Bitmap* image = NULL;
	std::vector<ICONDIRENTRY>	icon_entrys;

	ICONHEADER icon_header;	if (!read_buffer.read_bytes((unsigned char*)&icon_header, sizeof(icon_header)))
	{
		goto failed;
	}

	icon_header.idType = icon_header.idType;
	icon_header.idCount = icon_header.idCount;

	for (int i = 0; i < icon_header.idCount; ++i)
	{
		ICONDIRENTRY icon;
		if (!read_buffer.read_bytes((unsigned char*)&icon, sizeof(icon)))
		{
			goto failed;
		}

		icon_entrys.push_back(icon);
	}

	// 查找32位色的最佳匹配
	ICONDIRENTRY* best_icon = NULL;
	for (int i = 0; i < (int)icon_entrys.size(); ++i)
	{
		ICONDIRENTRY& icon = icon_entrys[i];
		if (icon.wBitCount == 32 || icon.bColorCount == 32)
		{
			if (icon.bWidth == icon_size)
			{
				best_icon = &icon;
				break;
			}
		}
	}
	if (best_icon == NULL)
	{
		std::sort(icon_entrys.begin(),icon_entrys.end(), SortIconEntry);
		for (int i = 0; i < (int)icon_entrys.size(); ++i)
		{
			ICONDIRENTRY& icon = icon_entrys[i];
			if (icon.wBitCount == 32)
			{
				if (icon.bWidth >= icon_size)
				{
					best_icon = &icon;
					break;
				}
			}
		}
	}

	if (best_icon == NULL)
	{
		HICON hIcon = NULL;
		int offset = LookupIconIdFromDirectoryEx((PBYTE)icon_data, TRUE, icon_size, icon_size, LR_DEFAULTCOLOR);     
		if (offset != 0) {     
			hIcon = CreateIconFromResourceEx((PBYTE)icon_data + offset, 0, TRUE, 0x30000, icon_size, icon_size, LR_DEFAULTCOLOR);     
		}     
		//HICON hIcon = CreateIconFromResourceEx((PBYTE)icon_data, icon_data_size, TRUE, 0x00030000, icon_size, icon_size, LR_DEFAULTCOLOR);
		if (hIcon)
		{
			Gdiplus::Bitmap* image = new Gdiplus::Bitmap(hIcon);
			DestroyIcon(hIcon);
			if (image)
			{
				read_buffer.detach();
				return image;
			}
		}
		goto failed;
	}

	if (!read_buffer.set_pos(best_icon->dwImageOffset))
	{
		goto failed;
	}

	// 读取头
	BITMAPINFOHEADER header;
	if (!read_buffer.read_bytes((unsigned char*)&header, sizeof(header)))
	{
		goto failed;
	}

	int width = best_icon->bWidth;
	int height = best_icon->bWidth;
	if (height < 0) height = -height;
	int data_size = 4 * width * height;
	data = (unsigned char*)malloc(data_size);
	if (data == NULL)
	{
		goto failed;
	}

	if (!read_buffer.read_bytes((unsigned char*)data, data_size))
	{
		goto failed;
	}

	{
		image = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

		// 图像是反的，要调转一次
		unsigned char* buffer = (unsigned char*)malloc(data_size);
		if (buffer == NULL)
		{
			goto failed;
		}
		int pitch = 4 * width;
		for (int i = 0; i < height; ++i)
		{
			memcpy(buffer+(pitch*(height-i-1)), data+(pitch*i), pitch);
		}

		Gdiplus::BitmapData data2;
		Gdiplus::Rect rc(0,0,width,height);
		data2.Scan0 = (void*)buffer;
		data2.Stride = 4 * width;
		if (Gdiplus::Ok == image->LockBits(&rc, 
			Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf, 
			PixelFormat32bppARGB, &data2))
		{
			image->UnlockBits(&data2);
		}
		free(buffer);
	}

	if (data)
	{
		free(data);
		data = NULL;
	}

	read_buffer.detach();

	if (width == icon_size && height == icon_size)
	{
		return image;
	}

	{
		Gdiplus::Bitmap* image2 = new Gdiplus::Bitmap(icon_size, icon_size, PixelFormat32bppARGB);
		Gdiplus::Graphics g(image2);

		g.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
		g.DrawImage(image, Gdiplus::Rect(0,0,icon_size,icon_size), 0,0,width,height, Gdiplus::UnitPixel);

		delete image;
		return image2;
	}

failed:

	read_buffer.detach();

	if (data) free(data);
	return NULL;
}


