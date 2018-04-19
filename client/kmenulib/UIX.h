
#ifndef __UIX_H__
#define __UIX_H__

#include "afxstr.h"
#include <Windows.h>

CRect GetTrayRect(CPoint pt);

Gdiplus::Rect CenterRect(Gdiplus::Rect ref_rc, Gdiplus::Rect rc);

bool WriteFileData(CString filename, void* data, unsigned int size);
bool ReadFileData(CString filename, unsigned char*& data, unsigned int& size);

CString GetFileExt(CString filepath);
CString GetFileName(CString filepath);
CString GetFilePath(CString filepath);

bool CopyFile(CString dst, CString src);

HANDLE BytesToHandle (const char *szText, int nTextLen);

HANDLE UnicodeToHandle (const wchar_t *szText, int nTextLen);


#endif

