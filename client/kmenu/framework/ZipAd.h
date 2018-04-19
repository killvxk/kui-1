#pragma once

#include "ResourceSystem/ZipSkin.h"

class CZipAd
{
public:
	CZipAd()
	{
	}
	~CZipAd()
	{
		if (m_hZip != NULL)
		{
			AfxCloseZipU(m_hZip);
			m_hZip = NULL;
		}
		if (m_pZipFile != NULL)
		{
			delete []m_pZipFile;
			m_pZipFile = NULL;
		}
	}
public:

	static CZipAd* getInstance();

	bool LoadZip(const CString& strPath);

	void UnLoad();

	bool FindFileFromZip(LPCTSTR lpFile);
	Gdiplus::Bitmap* LoadImageFromZip(LPCTSTR lpFile);
	bool LoadFileFromZipEx(LPCTSTR lpFile, unsigned char** buffer, unsigned int* size);

private:
	TCHAR szFile[MAX_PATH];
	HZIP  m_hZip;
	char* m_pZipFile;
};
