#include "ResourceSystem/ZipSkin.h"

CZipSkin* CZipSkin::getInstance()
{
	static CZipSkin skin;
	return &skin;
}

bool CZipSkin::LoadZip(const CString& strPath)
{
	m_hZip = NULL;
	m_pZipFile = NULL;
	TCHAR szFile[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFile, sizeof(szFile) - 1);
	PathRemoveFileSpec(szFile);
	PathAddBackslash(szFile);
	_tcscat_s(szFile, strPath);
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		m_pZipFile = new char[dwSize + 1];
		DWORD dwReadBytes = 0;
		ReadFile(hFile, m_pZipFile, dwSize, &dwReadBytes, NULL);
		m_pZipFile[dwSize] = 0;
		CloseHandle(hFile);
		m_hZip = AfxOpenZipU(m_pZipFile, dwSize, ZIP_MEMORY);
		if (m_hZip != NULL)
		{
			return true;
		}
	}
	return false;
}

void CZipSkin::UnLoad()
{
	if (m_hZip != NULL)
	{
		AfxCloseZipU(m_hZip);
		m_hZip = NULL;
	}
	if (m_pZipFile != NULL)
	{
		delete[]m_pZipFile;
		m_pZipFile = NULL;
	}
}

bool CZipSkin::FindFileFromZip(LPCTSTR lpFile)
{
	if (lpFile == NULL || m_hZip == NULL) return false;

	ZIPENTRYW ze = { 0 };
	int idx = 0;
	ZRESULT zr = AfxFindZipItem(m_hZip, lpFile, TRUE, &idx, &ze);
	if (zr == ZR_OK)
	{
		return true;
	}

	return false;
}

Gdiplus::Bitmap* CZipSkin::LoadImageFromZip(LPCTSTR lpFile)
{
	if (lpFile == NULL || m_hZip == NULL) return NULL;

	ZIPENTRYW ze = { 0 };
	int idx = 0;
	ZRESULT zr = AfxFindZipItem(m_hZip, lpFile, TRUE, &idx, &ze);
	if (zr == ZR_OK)
	{
		HGLOBAL hGlobal = GlobalAlloc(GMEM_FIXED, ze.unc_size);
		if (hGlobal != NULL)
		{
			void* pVoid = GlobalLock(hGlobal);
			if (pVoid != NULL)
			{
				AfxUnzipItem(m_hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
				GlobalUnlock(hGlobal);

				IStream* pIStream = NULL;
				DWORD dwError = CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream);
				if (pIStream != NULL)
				{
					Gdiplus::Bitmap* pImage = new Gdiplus::Bitmap(pIStream, FALSE);

					if (pImage->GetLastStatus() == Gdiplus::Ok)
					{
						pIStream->Release();
						return pImage;
					}
					if (pImage != NULL)
					{
						delete pImage;
						pImage = NULL;
					}
					pIStream->Release();
				}
			}

			GlobalFree(hGlobal);
		}
	}

	return NULL;
}
bool CZipSkin::LoadFileFromZipEx(LPCTSTR lpFile, unsigned char** buffer, unsigned int* size)
{
	if (lpFile == NULL || m_hZip == NULL) return false;

	ZIPENTRYW ze = { 0 };
	int idx = 0;
	ZRESULT zr = AfxFindZipItem(m_hZip, lpFile, TRUE, &idx, &ze);
	if (zr == ZR_OK)
	{
		*buffer = (unsigned char*)malloc(ze.unc_size);
		if (*buffer == NULL)
		{
			return false;
		}

		zr = AfxUnzipItem(m_hZip, ze.index, *buffer, ze.unc_size, ZIP_MEMORY);
		*size = ze.unc_size;
		return true;
	}

	return false;
}

