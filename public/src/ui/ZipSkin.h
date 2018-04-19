#pragma once

#include <afxwin.h>
#include <afxctl.h>
#include <afxhtml.h>
#include <atlimage.h>
#include <vector>
#include <gdiplus.h>
#include "kcore.h"

using namespace kcore;

#pragma comment(lib,"gdiplus.lib")

#ifndef _WIN64
#pragma comment(lib,"kcore.lib")
#else
#pragma comment(lib,"kcore64.lib")
#endif

class CZipSkinEx
{
public:
	CZipSkinEx()
	{
	}
	~CZipSkinEx()
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

	static CZipSkinEx* getInstance();

	BOOL   LoadSkinInfo(const CString& strSkinInfo)
	{
		CString strSkinFile;
		//AfxExtractSubString(strSkinFile, strSkinInfo, 1, TEXT('|'));
		//strSkinFile = CString(TEXT("Skin\\")) + strSkinFile;

		TCHAR szLanguage[MAX_PATH] = {0};
		AfxGetSysString(TEXT("config"), TEXT("language"), szLanguage, _countof(szLanguage));
		if (lstrlen(szLanguage) == 0)
		{
			strSkinFile = _T("skin\\Skin-chs.zip");
		}
		else
		{
			strSkinFile = CString(_T("skin\\Skin-")) + szLanguage + _T(".zip");
		}

		if(!LoadZip(strSkinFile))
		{
			if (!LoadZip(TEXT("skin\\Skin-chs.zip")))
			{
				if (!LoadZip(TEXT("skin\\Skin.zip")))
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	void ProcessImage(ATL::CImage* pImage)
	{
		if(pImage != NULL && pImage->GetBPP() == 32)
		{
			for(int i = 0; i < pImage->GetWidth(); ++i)
			{
				for(int j = 0; j < pImage->GetHeight(); ++j)
				{
					byte* pucColor = (byte* )pImage->GetPixelAddress(i, j);
					pucColor[0] = pucColor[0] * pucColor[3] / 255;
					pucColor[1] = pucColor[1] * pucColor[3] / 255;
					pucColor[2] = pucColor[2] * pucColor[3] / 255;
				}
			}
		}
	}

	BOOL   LoadZip(const CString& strPath)
	{
		m_hZip = NULL;
		m_pZipFile = NULL;
		TCHAR szFile[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFile, sizeof(szFile)-1);
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
				return TRUE;
			}
		}
		return FALSE;
	}
	void UnLoad()
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

	ATL::CImage* LoadImageFromFile(LPCTSTR FileName)
	{
		if (FileName == NULL) return NULL;

		TCHAR szExePath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szExePath, _countof(szExePath));
		PathRemoveFileSpec(szExePath);
		TCHAR szFileName[MAX_PATH] = {0};
		_stprintf_s(szFileName,MAX_PATH, TEXT("%s\\skin\\%s"), szExePath, FileName);

		ATL::CImage* pImage = new ATL::CImage;
		if (SUCCEEDED(pImage->Load(szFileName)))
		{
			ProcessImage(pImage);
			return pImage;
		}

		delete pImage;
		return NULL;
	}

	Gdiplus::Image* LoadImageFromFileEx(LPCTSTR FileName)
	{
		if (FileName == NULL) return NULL;

		TCHAR szExePath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szExePath, _countof(szExePath));
		PathRemoveFileSpec(szExePath);
		TCHAR szFileName[MAX_PATH] = {0};
		_stprintf_s(szFileName,MAX_PATH, TEXT("%s\\skin\\%s"), szExePath, FileName);

		Gdiplus::Image* pImage = new Gdiplus::Image(szFileName);
		if (pImage->GetLastStatus() == Gdiplus::Ok)
		{
			return pImage;
		}

		delete pImage;
		return NULL;
	}

	// 从资源ID获取
	ATL::CImage* LoadSkinFromResId(DWORD dwResId, LPCTSTR lpszResType = TEXT("PNG"))
	{
		HGLOBAL hGlobal = NULL;
		{
			HMODULE hMod = AfxGetInstanceHandle();
			HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(dwResId), lpszResType);
			if (hRes != NULL)
			{
				HGLOBAL hResGlobal = LoadResource(hMod, hRes);
				if (hResGlobal != NULL)
				{
					LPVOID  lpData = LockResource(hResGlobal);
					if (lpData != NULL)
					{
						DWORD dwSize = SizeofResource(hMod, hRes);
						hGlobal = GlobalAlloc(GMEM_FIXED, dwSize);
						LPVOID lpVoid = GlobalLock(hGlobal);
						if (lpVoid == NULL)
						{
							GlobalFree(hGlobal);
							hGlobal = NULL;
						}
						else
						{
							memcpy(lpVoid, lpData, dwSize);
							GlobalUnlock(hGlobal);
						}
					}
				}
			}
		}

		if (hGlobal != NULL)
		{
			IStream* pIStream = NULL;
			DWORD dwError = CreateStreamOnHGlobal(hGlobal, TRUE, &pIStream);
			if (pIStream != NULL)
			{
				ATL::CImage* pImage = new ATL::CImage;
				if(SUCCEEDED(pImage->Load(pIStream)))
				{
					pIStream->Release();
					ProcessImage(pImage);
					return pImage;
				}
				pIStream->Release();
			}
			GlobalFree(hGlobal);
		}

		return NULL;
	}

	ATL::CImage* LoadImageFromZip(LPCTSTR lpFile)
	{
		if(lpFile == NULL || m_hZip == NULL) return NULL;

		ZIPENTRYW ze = {0};
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
						ATL::CImage* pImage = new ATL::CImage;
						if(SUCCEEDED(pImage->Load(pIStream)))
						{
							pIStream->Release();
							ProcessImage(pImage);
							return pImage;
						}
						if(pImage != NULL)
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

	Gdiplus::Image* LoadImageFromZipEx(LPCTSTR lpFile)
	{
		if(lpFile == NULL || m_hZip == NULL) return NULL;

		ZIPENTRYW ze = {0};
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
						Gdiplus::Image* pImage = new Gdiplus::Image(pIStream, FALSE);

						if (pImage->GetLastStatus() == Gdiplus::Ok)
						{
							pIStream->Release();
							return pImage;
						}
						if(pImage != NULL)
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

	HICON LoadIconFromZip(LPCTSTR szPicture)
	{
		if (szPicture == NULL || m_hZip == NULL) return NULL;
		ZIPENTRYW ze = {0};

		HICON hIcon = NULL;
		int idx = 0;
		ZRESULT zr = AfxFindZipItem(m_hZip, szPicture, TRUE, &idx, &ze);
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

					/*CIcon ico;
					char* pData = (char* )pVoid;
					pData += 4;
					if(ico.CreateFromBuff((LPBYTE)pData, ze.unc_size))
					{ 
						hIcon = ico.GetHICON();
					}*/

					IStream* pIStream = NULL;
					Gdiplus::Bitmap* pImage = NULL;
					DWORD dwError = CreateStreamOnHGlobal(hGlobal, FALSE, &pIStream);
					if (pIStream != NULL)
					{
						pImage = new Gdiplus::Bitmap(pIStream);
						if (pImage != NULL)
						{
							pImage->GetHICON(&hIcon);
						}
						pIStream->Release();
					}
				}
				GlobalFree(hGlobal);
			}
		}

		return hIcon;
	}

	BOOL ExtractItem(LPCTSTR szItem, LPCTSTR szFilePath)
	{
		if (szItem == NULL || m_hZip == NULL) return FALSE;

		ZIPENTRYW ze = {0};
		HICON hIcon = NULL;
		BOOL  bRet = FALSE;
		int idx = 0;
		ZRESULT zr = AfxFindZipItem(m_hZip, szItem, TRUE, &idx, &ze);
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
					HANDLE hFile = ::CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) 
					{
						DWORD dwByteWrite = 0;
						if (WriteFile(hFile, pVoid, ze.unc_size, &dwByteWrite, NULL) && dwByteWrite == ze.unc_size)
						{
							bRet = TRUE;
						}
						SetEndOfFile(hFile);
						CloseHandle(hFile);
					}
				}
				GlobalFree(hGlobal);
			}
		}

		return bRet;
	}

private:
	TCHAR szFile[MAX_PATH];
	HZIP  m_hZip;
	char* m_pZipFile;
};


Gdiplus::Image* AfxUIGetImageEx(LPCTSTR FileName);

Gdiplus::Image* CombineImage(Gdiplus::Image* normal_image, Gdiplus::Image* hover_image = NULL, Gdiplus::Image* press_image = NULL, Gdiplus::Image* disable_image = NULL);
Gdiplus::Image* CombineImage(LPCTSTR normal_image, LPCTSTR hover_image = NULL, LPCTSTR press_image = NULL, LPCTSTR disable_image = NULL);

Gdiplus::Image* CombineImageEx(LPCTSTR image1, ...);
Gdiplus::Image* CombineImageResizeEx(int icon_width, int icon_height, LPCTSTR image1, ...);

bool SaveImage(const TCHAR* filename, Gdiplus::Image* image, int quality = 90);

Gdiplus::Image* ReadIcon(LPCTSTR iconfile, int icon_size);
Gdiplus::Image* ReadIcon(void* data, unsigned int data_size, int icon_size);


