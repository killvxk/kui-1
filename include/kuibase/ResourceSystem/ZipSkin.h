#pragma once

#include <SDKDDKVer.h>
#include <afxwin.h>
#include <gdiplus.h>

#pragma comment(lib,"gdiplus.lib")


//////////////////////////////////////////////////////////////////////////
//zip helper function.
DECLARE_HANDLE(HZIP);
typedef DWORD ZRESULT;

//ZIP FILE FLAG
#define ZIP_HANDLE   1
#define ZIP_FILENAME 2
#define ZIP_MEMORY   3
#define ZIP_FOLDER   4

#define ZR_OK         0x00000000     // nb. the pseudo-code zr-recent is never returned,
#define ZR_RECENT     0x00000001     // but can be passed to FormatZipMessage.
#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100     // couldn't duplicate the handle
#define ZR_NOFILE     0x00000200     // couldn't create/open the file
#define ZR_NOALLOC    0x00000300     // failed to allocate some resource
#define ZR_WRITE      0x00000400     // a general error writing to the file
#define ZR_NOTFOUND   0x00000500     // couldn't find that file in the zip
#define ZR_MORE       0x00000600     // there's still more data to be unzipped
#define ZR_CORRUPT    0x00000700     // the zipfile is corrupt or not a zipfile
#define ZR_READ       0x00000800     // a general error reading the file
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     // general mistake with the arguments
#define ZR_NOTMMAP    0x00020000     // tried to ZipGetMemory, but that only works on mmap zipfiles, which yours wasn't
#define ZR_MEMSIZE    0x00030000     // the memory size is too small
#define ZR_FAILED     0x00040000     // the thing was already failed when you called this function
#define ZR_ENDED      0x00050000     // the zip creation has already been closed
#define ZR_MISSIZE    0x00060000     // the indicated input file size turned out mistaken
#define ZR_PARTIALUNZ 0x00070000     // the file had already been partially unzipped
#define ZR_ZMODE      0x00080000     // tried to mix creating/opening a zip 
#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     // initialisation didn't work
#define ZR_SEEK       0x02000000     // trying to seek in an unseekable file
#define ZR_NOCHANGE   0x04000000     // changed its mind on storage, but not allowed
#define ZR_FLATE      0x05000000     // an internal error in the de/inflation code

typedef struct
{
	int index;
	char name[MAX_PATH];
	DWORD attr;
	FILETIME atime, ctime, mtime;
	long comp_size;
	long unc_size;
} ZIPENTRYA;

typedef struct
{
	int index;
	WCHAR name[MAX_PATH];
	DWORD attr;
	FILETIME atime, ctime, mtime;
	long comp_size;
	long unc_size;
} ZIPENTRYW;

__declspec(dllexport) HZIP	AfxOpenZipU(void *z, unsigned int len, DWORD flags);
__declspec(dllexport) ZRESULT AfxGetZipItemA(HZIP hz, int index, ZIPENTRYA *ze);
__declspec(dllexport) ZRESULT AfxGetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
__declspec(dllexport) ZRESULT AfxFindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYA *ze);
__declspec(dllexport) ZRESULT AfxFindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
__declspec(dllexport) ZRESULT AfxUnzipItem(HZIP hz, int index, void* dst, unsigned int len, DWORD flags);
__declspec(dllexport)ZRESULT AfxCloseZipU(HZIP hz);
DWORD	AfxFormatZipMessageU(ZRESULT code, char *buf, DWORD len);

#ifdef _UNICODE
#define ZIPENTRY	ZIPENTRYW
#define AfxGetZipItem	AfxGetZipItemW
#define AfxFindZipItem	AfxFindZipItemW
#else
#define ZIPENTRY ZIPENTRYA
#define AfxGetZipItem	AfxGetZipItemA
#define AfxFindZipItem	AfxFindZipItemA
#endif // _UNICODE

class __declspec(dllexport) CZipSkin
{
public:
	CZipSkin()
	{
	}
	~CZipSkin()
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

	static CZipSkin* getInstance();

	bool LoadZip(const CString& strPath);

	void UnLoad();

	bool FindFileFromZip(LPCTSTR lpFile);
	Gdiplus::Bitmap* LoadImageFromZip(LPCTSTR lpFile);
	bool LoadFileFromZipEx(LPCTSTR lpFile, unsigned char** buffer, unsigned int* size);

private:

private:
	TCHAR szFile[MAX_PATH];
	HZIP  m_hZip;
	char* m_pZipFile;
};
