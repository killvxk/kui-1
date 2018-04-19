#include "StdAfx.h"
#include "CchannelMgr.h"
#include <process.h>
#include <AtlEnc.h>
#include <time.h>
#include "channelsvr.h"
#include "channelclt.h"
#include <winhttp.h>
#include <Iphlpapi.h>
#pragma comment(lib, "winhttp.lib")
#include "winhttp\win_http.hpp"
#import "msxml3.dll"

#define _RUN_TMP_FILE_			TEXT(".~tmp")
HANDLE	g_hChannelLogger = NULL;

CchannelMgr::CchannelMgr(void)
{
	m_bType = TRUE;
	m_hExited = NULL;
	m_hThread = NULL;
	m_nReportLog = 0;
}

CchannelMgr::~CchannelMgr(void)
{

}
CchannelMgr& CchannelMgr::GetInstance()
{
	static CchannelMgr channelmgr;
	return channelmgr;
}

bool CchannelMgr::StartchannelMgr(DWORD_PTR reserved)
{
	if (m_hThread != NULL)
		return false;

	g_hChannelLogger = AfxCreateLogger(_T("kchannel"));
	AfxWriteLogger(g_hChannelLogger, _T("================================="));

	m_hExited = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_hExited == NULL)
		return false;

	TCHAR szKey[MAX_PATH] = {0};
	GetModuleFileName(NULL, szKey, _countof(szKey));
	LPTSTR szExeName = PathFindFileName(szKey);
	LPTSTR szExtName = PathFindExtension(szKey);
	if (szExeName == NULL || szExtName == NULL || szExtName <= szExeName)
		throw 0;
	lstrcpyn(szKey, szExeName, (int)(szExtName - szExeName + 1));
#ifndef _WIN64
	if (lstrcmpi(szKey, TEXT("knbserver")) == 0)
#else
	if (lstrcmpi(szKey, TEXT("knbserver64")) == 0)
#endif
	{
		m_bType = TRUE;
		sChannelSvr.StartServer();
		m_hThread = (HANDLE)::_beginthreadex(NULL, 0, WorkThreadSvr, this, 0, NULL);
	}
#ifndef _WIN64
	else if (lstrcmpi(szKey, TEXT("knbclient")) == 0)
#else
	else if (lstrcmpi(szKey, TEXT("knbclient64")) == 0)
#endif
	{
		m_bType = FALSE;
		sChannelClt.StartServer();
	}

	return true;
}

void CchannelMgr::StopchannelMgr()
{
	if(m_bType)
		sChannelSvr.StopServer();
	else
		sChannelClt.StopServer();
	if (m_hThread != NULL)
	{
		if (m_hExited != NULL)
		{
			::SetEvent(m_hExited);
			::WaitForSingleObject(m_hThread, INFINITE);

			::CloseHandle(m_hExited);
			m_hExited = NULL;

			::CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}

	ReleaseTask();

	AfxWriteLogger(g_hChannelLogger, _T("================================="));
	AfxCloseLogger(g_hChannelLogger);
}
void AfxGetAllDirectory(LPCTSTR szDirectory, std::vector<tstring>& dirlist, HANDLE hExit)
{
	WIN32_FIND_DATA wfd = {0};
	TCHAR szPath[MAX_PATH] = {0};
	_stprintf_s(szPath, _countof(szPath), TEXT("%s*"), szDirectory);
	HANDLE hFile = FindFirstFile(szPath, &wfd);
	if (hFile == INVALID_HANDLE_VALUE)
		return ;

	dirlist.push_back(szDirectory);
	do
	{
		if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
			break;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmpi(wfd.cFileName, TEXT(".")) != 0 && lstrcmpi(wfd.cFileName, TEXT("..")) != 0)
			{
				tstring subdir = tstring(szDirectory) + wfd.cFileName + TEXT("\\");
				AfxGetAllDirectory(subdir.c_str(), dirlist, hExit);
			}
		}
	} while (FindNextFile(hFile, &wfd));
	FindClose(hFile);
}
void AfxDeleteTempFile(LPCTSTR szDirectory, LPCTSTR szFilter, HANDLE hExit /*= NULL*/)
{
	TCHAR szRoot[MAX_PATH] = {0}, szPath[MAX_PATH] = {0};
	lstrcpy(szRoot, szDirectory);
	PathAddBackslash(szRoot);

	bool bKillprocess = false;
	std::vector<tstring> subdirs;
	AfxGetAllDirectory(szRoot, subdirs, hExit);
	for (std::vector<tstring>::iterator it = subdirs.begin(); it != subdirs.end(); it++)
	{
		if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
			break;
		WIN32_FIND_DATA wfd = {0};
		_stprintf_s(szPath, _countof(szPath), TEXT("%s%s"), it->c_str(), szFilter);
		HANDLE hFile = FindFirstFile(szPath, &wfd);
		if (hFile == INVALID_HANDLE_VALUE)
			continue ;

		do 
		{
			if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
				break;
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				tstring file = (*it) +wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				BOOL bSucess = DeleteFile(file.c_str());
				if(!bSucess)
				{
					AfxWriteLogger(g_hChannelLogger, TEXT("delete file faild:%s,%d"), file.c_str(),GetLastError());
					AfxKillProcess(_T("kconsole.exe"));
					AfxKillProcess(_T("ServiceManage.exe"));
					bKillprocess = true;
					Sleep(2000);
					SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
					BOOL bDeleteAgain = DeleteFile(file.c_str());
					if(!bDeleteAgain)
						AfxWriteLogger(g_hChannelLogger, TEXT("kill process delete file faild:%s,%d"), file.c_str(),GetLastError());
				}
			}
		} while (FindNextFile(hFile, &wfd));
		FindClose(hFile);
	}

	if(bKillprocess)
	{
		AfxRunProcess(TEXT("kconsole.exe"));
		AfxRunProcess(TEXT("ServiceManage.exe"));
	}
}

UINT __stdcall CchannelMgr::WorkThreadSvr( LPVOID lpVoid )
{
	CchannelMgr *pThis = reinterpret_cast<CchannelMgr*>(lpVoid);

	bool bDeleteTemp = false;
	
	bool bInit = S_OK == ::CoInitialize(NULL);
	DWORD nTime = 2 * 60 * 60 * 1000;
	do 
	{
		nTime = 6000;

		pThis->GetReportLogConfigFromFile();

		if(!pThis->GetNetBarIdName())
			continue;
		else
			pThis->ReportIdcPlugLog(pThis->m_nNetBarId,0,FT_SERVER_ACTIVE,pThis->GetMacAddress());

		if(!bDeleteTemp)
		{
			TCHAR szPath[MAX_PATH] = {0};
			AfxDeleteTempFile(AfxGetAppPath(szPath), TEXT("*")_RUN_TMP_FILE_,pThis->m_hExited);
			bDeleteTemp = true;
		}

		if (::WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
			break;

		std::string strXml;
		if(pThis->GetAllTask(strXml))
		{
			if (::WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
				break;

			if(pThis->ParseXml(strXml))
			{
				if (::WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
					break;

				pThis->CheckNeedDownTask();

				if (::WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
					break;

				pThis->DeleteNotUseTask();

				if (::WaitForSingleObject(pThis->m_hExited, 0) == WAIT_OBJECT_0)
					break;

				pThis->DownNeedDownTask();

				nTime = 2 * 60 * 60 * 1000;
			}
		}

	} while (WaitForSingleObject(pThis->m_hExited,nTime) == WAIT_TIMEOUT);
	if (bInit)
		CoUninitialize();
	return 0;
}


bool CchannelMgr::GetAllTask(std::string& strXml)
{
	bool		bResult = true;

	AfxWriteLogger(g_hChannelLogger, TEXT("query channel all task list."));
	try
	{
		tstringstream	stream;
		stream << TEXT("nid=") << m_nNetBarId << TEXT("&username=") << m_strNetbarName.c_str();
		
		struct handler_t
		{
			void operator()(const std::string &msg)
			{
				throw std::runtime_error(msg);
			}
		};

		TCHAR szUrl[MAX_PATH] = { 0 };
		GetLocalStringFun(szUrl, MAX_PATH, _T("IDCTASK_URL"), _T("http://interface.yungengxin.com/channel/getTasks"));
		http::win_http_t win_http(szUrl, http::post, handler_t());
		std::string params = (LPCSTR) ATL::CW2A(stream.str().c_str(), CP_UTF8);

		std::map<std::wstring, std::wstring> header;
		header[L"Connection"] = L"close";
		header[L"Content-type"] = L"application/x-www-form-urlencoded";
		std::vector<char> data = win_http.send(header, params);
		std::string response_val(data.begin(), data.end());
		strXml = response_val;

		if (strXml.substr(0, 5) == "flag=")
		{
			bResult = false;
			AfxWriteLogger(g_hChannelLogger, _T("response %d, %s, %s"), m_nNetBarId, m_strNetbarName.c_str(), CA2W(strXml.c_str(), CP_UTF8));
		}
		else
		{
			AfxWriteLogger(g_hChannelLogger, _T("response %d, %s, data size %d"), m_nNetBarId, m_strNetbarName.c_str(), strXml.size());
		}
	}
	catch( const std::exception &e )
	{
		std::wstring msg = CA2W(e.what());
		AfxWriteLogger(g_hChannelLogger, TEXT("get channel task list Error:%s"), msg.c_str());
		bResult = false;
	}

	return bResult;
}

bool CchannelMgr::GetNetBarIdName()
{
	bool bResult = true;
	SOCKET sck = AfxSocketConnect(_T("127.0.0.1"),DB_TCP_PORT);
	if(sck != INVALID_SOCKET)
	{
		try
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_DB_SQL, OBJECT_UNKOWN);	
			pkgSend << _T("SELECT value FROM tOption WHERE name = \'uuserid\'");
			if (ERROR_SUCCESS != AfxSocketExecute(sck, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, _countof(szRecvBuffer)))
				throw 0;
			CPkgHelper pkgRecv(szRecvBuffer);
			if (pkgRecv.IsStatusOk())
			{
				int col = 0, row =0;
				TCHAR szNetBarId[MAX_PATH] = {0};
				pkgRecv >> row >> col;
				if(row == 1)
				{
					pkgRecv >> szNetBarId;
					m_nNetBarId = _ttol(szNetBarId);
				}
				else
					throw 0;
			}
			else
				throw 0;
		}
		catch(...)
		{ bResult = false;}

		if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
			return bResult;

		try
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_DB_SQL, OBJECT_UNKOWN);	
			pkgSend << _T("SELECT value FROM tOption WHERE name = \'uusername\'");
			if (ERROR_SUCCESS != AfxSocketExecute(sck, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, _countof(szRecvBuffer)))
				throw 0;
			CPkgHelper pkgRecv(szRecvBuffer);
			if (pkgRecv.IsStatusOk())
			{
				int col = 0, row =0;
				pkgRecv >> row >> col;
				if(row == 1)
				{
					TCHAR szName[MAX_PATH] = {0};
					pkgRecv >> szName;
					m_strNetbarName = szName;
				}
				else
					throw 0;
			}
			else
				throw 0;
		}
		catch(...)
		{ bResult = false;}

		AfxSocketClose(sck);
	}
	else
	{
		bResult = false;
	}
	return bResult;
}
std::string AfxBase64Decode(std::string strBase64)
{
	std::string		strDecoded;

	LPSTR		pszBase64 = (LPSTR)strBase64.c_str();

	int nRequiredLen = ATL::Base64DecodeGetRequiredLength((int)strlen(pszBase64));

	if (nRequiredLen > 0)
	{
		int			nDecodedLen = nRequiredLen + sizeof(char);
		BYTE*		pszDecoded = new BYTE[nDecodedLen];

		memset(pszDecoded, 0, nDecodedLen);

		if (ATL::Base64Decode(pszBase64, (int)strlen(pszBase64), (BYTE*)pszDecoded, &nDecodedLen))
			strDecoded = (LPCSTR)pszDecoded;

		delete []pszDecoded;
	}
	return strDecoded;
}
std::wstring AfxUTF82Unicode(LPCSTR  pUTF8)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pUTF8, -1, NULL, 0);
	std::wstring result;
	result.resize(nLen);
	MultiByteToWideChar(CP_UTF8, 0, pUTF8, -1, &result[0], nLen);
	if (result.size())
		result.resize(result.size()-1);
	return result;
}
bool CchannelMgr::ParseXml( std::string& strXml )
{
	bool bResult = true;
	try
	{
		MSXML2::IXMLDOMDocumentPtr xmlDoc(_uuidof(MSXML2::DOMDocument)); 
		if (VARIANT_TRUE == xmlDoc->loadXML((_bstr_t)strXml.c_str()))
		{
			MSXML2::IXMLDOMElementPtr XMLROOT;
			XMLROOT = xmlDoc->GetdocumentElement();
			MSXML2::IXMLDOMNodePtr nodeList  = XMLROOT->selectSingleNode(_T("/doc/tasklist"));
			MSXML2::IXMLDOMNodeListPtr ptr = nodeList->GetchildNodes();
			m_arAllTaskListFromIdc.clear();
			for (long idx=0; idx<ptr->length; idx++)
			{
				if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
					break;

				TASKLIST tasklist = {0};
				std::string strDecode;
				tstring strUnicode;
				MSXML2::IXMLDOMElementPtr node  = ptr->Getitem(idx);
				tstring str = (TCHAR*)(_bstr_t)node->getAttribute(_T("id"));
				tasklist.nTaskId = _ttoi(str.c_str());
				str = (TCHAR*)(_bstr_t)node->getAttribute(_T("type"));
				tasklist.nType = _ttoi(str.c_str());
				str = (TCHAR*)(_bstr_t)node->getAttribute(_T("ver"));
				tasklist.nTaskVer = _ttoi(str.c_str());
				str = (TCHAR*)(_bstr_t)node->getAttribute(_T("name"));
				strDecode = AfxBase64Decode((char*)(_bstr_t)str.c_str());
				strUnicode = AfxUTF82Unicode(strDecode.c_str());
				_tcscpy_s(tasklist.szName,strUnicode.c_str());
				str = (TCHAR*)(_bstr_t)node->getAttribute(_T("code"));
				_tcscpy_s(tasklist.szMd5,str.c_str());
				str = (TCHAR*)(_bstr_t)node->getAttribute(_T("url"));
				strDecode = AfxBase64Decode((char*)(_bstr_t)str.c_str());
				strUnicode = AfxUTF82Unicode(strDecode.c_str());
				_tcscpy_s(tasklist.szUrl,strUnicode.c_str());
				m_arAllTaskListFromIdc.push_back(tasklist);
			}
			AfxWriteLogger(g_hChannelLogger, _T("idc task count %d"), m_arAllTaskListFromIdc.size());
		}
		else
		{
			bResult = false;
			AfxWriteLogger(g_hChannelLogger, _T("load channel xml Error"));
		}
	}
	catch(_com_error& Error)
	{
		bResult = false;
		AfxWriteLogger(g_hChannelLogger, _T("Parser channel xml Error:%s"),(LPCTSTR)Error.Description());
	}

	return bResult;
}

bool CchannelMgr::CheckNeedDownTask()
{
	m_arNeedDownTaskList.clear();
	for (size_t i=0;i<m_arAllTaskListFromIdc.size();i++)
	{
		if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
			break;

		bool bFind = false;
		for(size_t j=0;j<m_arAllTaskListLocal.size();j++)
		{
			if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				break;

			if(m_arAllTaskListFromIdc[i].nTaskId == m_arAllTaskListLocal[j].nTaskId )
			{
				bFind = true;
				if(m_arAllTaskListFromIdc[i].nTaskVer != m_arAllTaskListLocal[j].nTaskVer)
					m_arNeedDownTaskList.push_back(m_arAllTaskListFromIdc[i]);
				break;
			}
		}
		if(!bFind)
			m_arNeedDownTaskList.push_back(m_arAllTaskListFromIdc[i]);
	}

	m_arAllTaskListLocal = m_arAllTaskListFromIdc;
	AfxWriteLogger(g_hChannelLogger, _T("local task count %d"), m_arAllTaskListLocal.size());

	return true;
}

bool CchannelMgr::DeleteNotUseTask()
{
	OBJECT_LOCK(m_lockTaskZip);
	for (map<DWORD,TASKZIPFILE>::iterator itr = m_mapTaskZip.begin();itr != m_mapTaskZip.end();)
	{
		if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
			break;

		bool bFind = false;
		for(size_t i=0;i<m_arAllTaskListLocal.size();i++)
		{
			if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				break;

			if (itr->first == m_arAllTaskListLocal[i].nTaskId)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind)
		{
			AfxWriteLogger(g_hChannelLogger, _T("remove task %d"), itr->first);
			Delete_Array(itr->second.pFileBuffer);
			itr = m_mapTaskZip.erase(itr);
			continue;
		}
		itr++;
	}
	return true;
}

void CchannelMgr::DownNeedDownTask()
{
	for (size_t i=0;i<m_arNeedDownTaskList.size();i++)
	{
		if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
			break;

		AfxWriteLogger(g_hChannelLogger, _T("start down task %d"), m_arNeedDownTaskList[i].nTaskId);

		bool bDown = false;
		//md5不对下两次就换下一个
		for(int nTimes = 0;nTimes < 2;nTimes++)
		{
			if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				break;

			char* pFile = NULL;DWORD dwFileSize = 0;
			if(DownSingleTask(m_arNeedDownTaskList[i].szUrl,pFile,dwFileSize))
			{
				TCHAR md5[MAX_PATH] = {0};
				AfxMD5Encode(pFile,dwFileSize,md5);
				if(_tcsicmp(md5,m_arNeedDownTaskList[i].szMd5) == 0)
				{
					ReportIdcPlugLog(m_nNetBarId,m_arNeedDownTaskList[i].nTaskId,FT_SERVER_DOWNTASK,GetMacAddress());

					TASKZIPFILE taskFile = {0};
					taskFile.nFileSize = dwFileSize;
					taskFile.nHashCode = _time32(NULL);
					taskFile.pFileBuffer = pFile;
					OBJECT_LOCK(m_lockTaskZip);
					map<DWORD,TASKZIPFILE>::iterator itr = m_mapTaskZip.find(m_arNeedDownTaskList[i].nTaskId);
					if(itr != m_mapTaskZip.end())
					{
						Delete_Array(itr->second.pFileBuffer);
						m_mapTaskZip.erase(itr);
					}
					m_mapTaskZip.insert(make_pair(m_arNeedDownTaskList[i].nTaskId,taskFile));
					bDown = true;
					break;
				}
				else
				{
					AfxWriteLogger(g_hChannelLogger, _T("md5 is not match, down again."));
				}
			}
		}
		if (!bDown)
			EraseVecLocalTask(m_arNeedDownTaskList[i].nTaskId);
	}
}

bool CchannelMgr::DownSingleTask( tstring strUrl,char*& pFile ,DWORD& dwFileSize)
{
	bool bSuccess = true;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	try
	{
		wchar_t host[MAX_PATH] = {0};
		wchar_t req [MAX_PATH] = {0};
		URL_COMPONENTS urlComp = {0};
		urlComp.dwStructSize = sizeof(urlComp);
		urlComp.lpszHostName = host;
		urlComp.dwHostNameLength  = MAX_PATH;
		urlComp.lpszUrlPath = req;
		urlComp.dwUrlPathLength   = MAX_PATH;
		WinHttpCrackUrl(strUrl.c_str(), (DWORD)strUrl.size(), 0, &urlComp);
		if ((hSession = ::WinHttpOpen(TEXT("A WinHTTP download Program/1.0"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)) == NULL)
			throw GetLastError();
		WinHttpSetTimeouts(hSession, 3000, 3000, 3000, 3000);
		if ((hConnect = ::WinHttpConnect(hSession, host, urlComp.nPort, 0)) == NULL)
			throw GetLastError();
		if ((hRequest = ::WinHttpOpenRequest(hConnect, TEXT("GET"), req, NULL, WINHTTP_NO_REFERER, 
			WINHTTP_DEFAULT_ACCEPT_TYPES, 0)) == NULL)
			throw GetLastError();
		if (!::WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
			throw GetLastError();
		if (!::WinHttpReceiveResponse(hRequest, NULL))
			throw GetLastError();

		DWORD dwStatus = 0;
		DWORD dwSize = sizeof(DWORD);
		if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER ,
			NULL, &dwStatus, &dwSize, WINHTTP_NO_HEADER_INDEX))
			throw GetLastError();
		if (dwStatus != HTTP_STATUS_OK)
		{
			AfxWriteLogger(g_hChannelLogger, TEXT("channel down fail:http status error:%s:%u"), strUrl.c_str(), dwStatus);
			throw (DWORD)0;
		}
		dwSize = sizeof(DWORD);
		if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH|WINHTTP_QUERY_FLAG_NUMBER,
			NULL, &dwFileSize, &dwSize, WINHTTP_NO_HEADER_INDEX) )
			throw GetLastError();

		/*if (dwFileSize >= 1 * 1024 * 1024)
		{
		AfxWriteLogger(ghLogger, TEXT("game list size:%s:0x%X"), url, dwFileSize);
		throw 0ul;
		}*/

		pFile = new char[ROUND_SIZE_4K(dwFileSize+1)];
		long dwLeft = dwFileSize;
		while (dwLeft > 0)
		{
			if (::WaitForSingleObject(m_hExited, 0) == WAIT_OBJECT_0)
				throw 0ul;
			DWORD dwReadBytes = 0;
			if (!::WinHttpQueryDataAvailable(hRequest, &dwReadBytes) ||
				!::WinHttpReadData(hRequest, pFile + dwFileSize - dwLeft, dwReadBytes, &dwReadBytes) || 
				dwReadBytes == 0)
			{
				throw GetLastError();
			}
			dwLeft -= dwReadBytes;
		}
		pFile[dwFileSize] = 0;
		bSuccess = true;

		//write zip to c:\gamelist\task-date.zip
		if (PathFileExists(TEXT("C:\\gamelist\\")))
		{
			TCHAR szFileName[MAX_PATH] = {0};
			SYSTEMTIME st = {0};
			GetLocalTime(&st);
			_stprintf_s(szFileName, _countof(szFileName), TEXT("c:\\gamelist\\task-%04d%02d%02d-%02d%02d%02d-%d.zip"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, GetTickCount());
			HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwBytes = 0;
				WriteFile(hFile, pFile, dwFileSize, &dwBytes, NULL);
				SetEndOfFile(hFile);
				CloseHandle(hFile);
			}
		}
	}
	catch (DWORD dwError)
	{
		Delete_Array(pFile);

		if (dwError)
		{
			_com_error Error(dwError);
			AfxWriteLogger(g_hChannelLogger, TEXT("Download channel file Error:%s:%d:%s"), 
				strUrl.c_str(), dwError, (LPCTSTR)Error.Description());
		}
		bSuccess = false;
	}

	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return bSuccess;
}

void CchannelMgr::GetTaskZipInfo( vector<TASKZIPINFO>& arVec )
{
	OBJECT_LOCK(m_lockTaskZip);
	for(map<DWORD,TASKZIPFILE>::iterator itr=m_mapTaskZip.begin();itr!=m_mapTaskZip.end();itr++)
	{
		TASKZIPINFO taskInfo = {0};
		taskInfo.nTaskId = itr->first;
		taskInfo.nHashCode = itr->second.nHashCode;
		taskInfo.nFileSize = itr->second.nFileSize;
		arVec.push_back(taskInfo);
	}
}

bool CchannelMgr::GetTaskZipFile( CPkgHelper& In, CPkgHelper& Out)
{
	DWORD nTaskId = 0,nOffSet = 0,nLenth = 0,nHashCode = 0;
	In >> nTaskId >> nOffSet >> nLenth >> nHashCode;

	OBJECT_LOCK(m_lockTaskZip);
	map<DWORD,TASKZIPFILE>::iterator itr = m_mapTaskZip.find(nTaskId);
	if(itr != m_mapTaskZip.end())
	{
		if(itr->second.nHashCode != nHashCode)
		{
			Out.PushErrorInfo(PKG_STA_ERROR, _T("task id file buf is change."));
			return false;
		}
		if(nOffSet + nLenth > itr->second.nFileSize ||
			nLenth > PKG_MAXLENGTH - sizeof(PkgHeader))
		{
			Out.PushErrorInfo(PKG_STA_ERROR, _T("length is out of buf range."));
			return false;
		}
		Out.PushPointer(itr->second.pFileBuffer+nOffSet,nLenth);
		return true;
	}
	else
		Out.PushErrorInfo(PKG_STA_ERROR, _T("task id is not exist."));

	return false;
}

void CchannelMgr::ReleaseTask()
{
	OBJECT_LOCK(m_lockTaskZip);
	map<DWORD,TASKZIPFILE>::iterator itr = m_mapTaskZip.begin();
	for(;itr != m_mapTaskZip.end();itr++)
		Delete_Array(itr->second.pFileBuffer);
	m_mapTaskZip.clear();

	m_arAllTaskListFromIdc.clear();
	m_arAllTaskListLocal.clear();
	m_arNeedDownTaskList.clear();
}

void CchannelMgr::ReportIdcPlugLog( DWORD nNetBarId,DWORD nTaskId,DWORD nType,tstring strMac/*=_T("00-00-00-00-00-00")*/ )
{
	if(m_nReportLog == 0 && nType != FT_CLIENT_RUNTASK)
		return;

	try
	{
		tstringstream	stream;
		stream << TEXT("nid=") << nNetBarId << TEXT("&mac=") << strMac.c_str();
		stream << TEXT("&taskIds=") << nTaskId << TEXT("&type=") << nType;

		struct handler_t
		{
			void operator()(const std::string &msg)
			{
				throw std::runtime_error(msg);
			}
		};

		TCHAR szUrl[MAX_PATH] = { 0 };
		GetLocalStringFun(szUrl, MAX_PATH, _T("REPORTLOG_URL"), _T("http://interface.yungengxin.com/channel/log"));
		http::win_http_t win_http(szUrl, http::post, handler_t());
		std::string params = (LPCSTR) ATL::CW2A(stream.str().c_str(), CP_UTF8);

		std::map<std::wstring, std::wstring> header;
		header[L"Connection"] = L"close";
		header[L"Content-type"] = L"application/x-www-form-urlencoded";
		std::vector<char> data = win_http.send(header, params);
		std::string response_val(data.begin(), data.end());
		tstring strXml = ATL::CA2W(response_val.c_str(),CP_UTF8);
		AfxWriteLogger(g_hChannelLogger, _T("response %s, nid:%d, taskIds:%d, type:%d"),strXml.c_str(), nNetBarId,nTaskId,nType);
		
	}
	catch( const std::exception &e )
	{
		std::wstring msg = CA2W(e.what());
		AfxWriteLogger(g_hChannelLogger, TEXT("report channel plug Error:%s"), msg.c_str());
		
	}
}

tstring CchannelMgr::GetMacAddress()
{
	tstring strMac;
	DWORD dwSize = 0;
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	if (GetAdaptersInfo(pAdapterInfo, &dwSize) != ERROR_BUFFER_OVERFLOW)
		return _T("");
	pAdapterInfo = (PIP_ADAPTER_INFO)(new char[dwSize]);
	if (GetAdaptersInfo(pAdapterInfo, &dwSize) != NOERROR)
	{
		delete [] (char*)pAdapterInfo;
		return _T("");
	}
	PIP_ADAPTER_INFO pAdapter = pAdapterInfo, pNextAdapter = pAdapterInfo;
	while(pNextAdapter)
	{
		pAdapter = pNextAdapter;
		pNextAdapter = pAdapter->Next;

		MIB_IFROW IfRow = {0};
		IfRow.dwIndex = pAdapter->Index;
		if (GetIfEntry(&IfRow) != NO_ERROR || inet_addr(pAdapter->IpAddressList.IpAddress.String) == 0 || 
			inet_addr(pAdapter->GatewayList.IpAddress.String) == 0 ||
			IfRow.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
			continue;

		TCHAR szMac[MAX_PATH] = {0};
		_stprintf_s(szMac, _countof(szMac), TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
			pAdapter->Address[0],
			pAdapter->Address[1],
			pAdapter->Address[2],
			pAdapter->Address[3],
			pAdapter->Address[4],
			pAdapter->Address[5]);

		strMac =  szMac;
		break;
	}
	delete [] (char*)pAdapterInfo;

	return strMac;
}

void CchannelMgr::EraseVecLocalTask( DWORD nTaskId )
{
	vector<TASKLIST>::iterator itr = m_arAllTaskListLocal.begin();
	for(;itr != m_arAllTaskListLocal.end();itr++)
	{
		if(itr->nTaskId == nTaskId)
		{
			m_arAllTaskListLocal.erase(itr);
			break;
		}
	}
}

void CchannelMgr::GetReportLogConfigFromFile()
{
	OBJECT_LOCK(m_lockLog);
	m_nReportLog = 0;
	TCHAR szPath[MAX_PATH] = {0};
	AfxGetAppPath(szPath,MAX_PATH,_T("config\\cgm_2015.ini"));
	if(PathFileExists(szPath))
		m_nReportLog = GetPrivateProfileInt(_T("config"),_T("report"), 0, szPath);
}

DWORD CchannelMgr::GetReportStyle()
{
	OBJECT_LOCK(m_lockLog);
	return m_nReportLog;
}

void CchannelMgr::SetReportStyle( DWORD nStyle )
{
	m_nReportLog = nStyle;
}



