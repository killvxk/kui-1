
#include <stdafx.h>
#include "UIX.h"
#include <stdio.h>

//获取普通托盘区窗口句柄  
HWND FindTrayWnd()  
{  
	HWND hWnd = NULL;  
	HWND hWndPaper = NULL;  

	if ((hWnd = FindWindow(_T("Shell_TrayWnd"), NULL)) != NULL)  
	{  
		if ((hWnd = FindWindowEx(hWnd, 0, _T("TrayNotifyWnd"), NULL)) != NULL)  
		{  
			hWndPaper = FindWindowEx(hWnd, 0, _T("SysPager"), NULL);  
			if(!hWndPaper)  
				hWnd = FindWindowEx(hWnd, 0, _T("ToolbarWindow32"), NULL);  
			else  
				hWnd = FindWindowEx(hWndPaper, 0, _T("ToolbarWindow32"), NULL);  
		}  
	}  

	return hWnd;  
}

//枚举获取托盘区域位置  
bool EnumNotifyWindow(RECT &rect,HWND hWnd,CPoint pt)  
{  
	//RECT rect = {0};  
	bool bSuc = false;  
	unsigned long lngPID = 0;  
	long ret = 0,lngButtons = 0;  
	long lngHwndAdr = 0,lngHwnd = 0;//,lngTextAdr,lngButtonID;  
	HANDLE hProcess = NULL;  
	LPVOID lngAddress = NULL,lngRect = NULL;  

	if (hWnd != NULL)  
	{  
		ret = GetWindowThreadProcessId(hWnd, &lngPID);  
		if(ret != 0 && lngPID != 0)  
		{  
			hProcess = OpenProcess(PROCESS_ALL_ACCESS|PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE,0,lngPID);//  
			if (hProcess != NULL)  
			{  
				lngAddress = VirtualAllocEx(hProcess,0, 0x4096, MEM_COMMIT, PAGE_READWRITE);  
				lngRect = VirtualAllocEx(hProcess,0,sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);  
				lngButtons = (long)SendMessage(hWnd, TB_BUTTONCOUNT, 0, 0); //发送消息获取托盘button数量  
				if (lngAddress != NULL  && lngRect != NULL)  
				{  
					for(int i=0 ;i< lngButtons;i++)  
					{  
						CRect rc(0,0,0,0);   

						ret = (long)::SendMessage(hWnd, TB_GETITEMRECT, (WPARAM)i, (LPARAM)lngRect); //发送消息获取托盘项区域数据  
						ret = ReadProcessMemory(hProcess,lngRect,&rc, sizeof(rc),0);  //读取托盘区域数据  
						if(ret != 0)  
						{
							CWnd::FromHandle(hWnd)->ClientToScreen(&rc);
							if (rc.PtInRect(pt))
							{
								rect = rc;
								bSuc = true;//在普通托盘区找到，在溢出区不再查找  
								break;
							}
						}
					}
				}
				if (lngAddress != NULL)  
				{  
					VirtualFreeEx( hProcess, lngAddress, 0x4096, MEM_DECOMMIT);  
					VirtualFreeEx( hProcess, lngAddress, 0, MEM_RELEASE);  
				}  
				if (lngRect != NULL)  
				{  
					VirtualFreeEx( hProcess, lngRect, sizeof(RECT), MEM_DECOMMIT);  
					VirtualFreeEx( hProcess, lngRect, 0, MEM_RELEASE);  
				}  
				CloseHandle(hProcess);  
			}  
		}  
	}  
	return bSuc;  
}

CRect GetTrayRect(CPoint pt)
{  
	RECT rect = {0};  
	HWND hWnd = NULL;  

	hWnd = FindTrayWnd();  
	if (hWnd != NULL)  
	{  
		if (!EnumNotifyWindow(rect,hWnd, pt))//如果没在普通托盘区  
		{  

		}  
	}  

	return rect;  
}  

Gdiplus::Rect CenterRect(Gdiplus::Rect ref_rc, Gdiplus::Rect rc)
{
	int x = ref_rc.X + ((ref_rc.Width - rc.Width) / 2);
	int y = ref_rc.Y + ((ref_rc.Height - rc.Height) / 2);

	return Gdiplus::Rect(x,y,rc.Width,rc.Height);
}

HANDLE BytesToHandle (const char *szText, int nTextLen)
{
	void  *ptr;
	// if text length is -1 then treat as a nul-terminated string
	if(nTextLen == -1)
		nTextLen = (int)(strlen (szText) + 1);
	// allocate and lock a global memory buffer. Make it fixed
	// data so we don't have to use GlobalLock
	HANDLE handle = GlobalAlloc(GMEM_MOVEABLE, nTextLen);
	ptr = (void *)GlobalLock(handle);
	// copy the string into the buffer
	memcpy (ptr, szText, nTextLen - 1);
	((char*)ptr)[nTextLen - 1] = '\0';
	GlobalUnlock(handle);
	return handle;
}

HANDLE UnicodeToHandle (const wchar_t *szText, int nTextLen)
{
	void  *ptr;
	// if text length is -1 then treat as a nul-terminated string
	if(nTextLen == -1)
		nTextLen = (int)wcslen (szText) + 1;
	// allocate and lock a global memory buffer. Make it fixed
	// data so we don't have to use GlobalLock
	HANDLE handle = GlobalAlloc(GMEM_MOVEABLE, nTextLen * 2);
	ptr = (void *)GlobalLock(handle);
	// copy the string into the buffer
	memcpy (ptr, szText, (nTextLen - 1) * 2);
	((wchar_t*)ptr)[nTextLen - 1] = '\0';
	GlobalUnlock(handle);
	return handle;
}
bool WriteFileData(CString filename, void* data, unsigned int size)
{
	FILE* f = NULL;
	if (_wfopen_s(&f,filename, L"wb") != 0)
		return false;
	size_t ret = fwrite(data, size, 1, f);
	if (ret != 1)
	{
		fclose(f);
		return false;
	}
	fclose(f);
	return true;
}
bool ReadFileData(CString filename, unsigned char*& data, unsigned int& size)
{
	FILE* f = NULL;
	if (_wfopen_s(&f,filename, L"rb") != 0)
		return false;

	if (f == NULL)
	{
		return false;
	}
	int ret = fseek(f, 0, SEEK_END);
	if (ret != 0)
	{
		fclose(f);
		return false;
	}
	long filesize = ftell(f);
	if (filesize <= 0)
	{
		fclose(f);
		return false;
	}
	ret = fseek(f, 0, SEEK_SET);
	if (ret != 0)
	{
		fclose(f);
		return false;
	}

	data = (unsigned char*)malloc(filesize + 1);
	if (data == NULL)
	{
		return false;
	}
	ret = (int)fread(data, filesize, 1, f);
	if (ret != 1)
	{
		free(data);
		fclose(f);
		return false;
	}
	fclose(f);
	data[filesize] = '\0';
	size = filesize;
	return true;
}

CString GetFileName(CString filepath)
{
	for (int i = filepath.GetLength() - 1; i >= 0; --i)
	{
		wchar_t c = filepath.GetAt(i);
		if (c == '\\' || c == '/')
		{
			return filepath.Mid(i + 1);
		}
	}
	return filepath;
}

CString GetFileExt(CString filepath)
{
	CString filename = GetFileName(filepath);
	int pos = filename.ReverseFind('.');
	if (pos >= 0)
	{
		return filename.Mid(pos + 1);
	}
	return CString();
}

CString GetFilePath(CString filepath)
{
	for (int i = filepath.GetLength() - 1; i >= 0; --i)
	{
		wchar_t c = filepath.GetAt(i);
		if (c == '\\' || c == '/')
		{
			--i;
			while (i >= 0)
			{
				wchar_t c = filepath.GetAt(i);
				if (c == '\\' || c == '/')
				{
					--i;
				}
				else
				{
					break;
				}
			}
			return filepath.Mid(0, i+1);
		}
	}
	return CString();
}

bool CopyFile(CString dst, CString src)
{
	if (src == dst) return true;

	unsigned char* data = NULL;
	unsigned int size = 0;
	if (!ReadFileData(src, data, size))
	{
		return false;
	}

	if (!WriteFileData(dst, data, size))
	{
		free(data);
		return false;
	}
	free(data);
	return true;
}