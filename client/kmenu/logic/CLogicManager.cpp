#include "HMI.h"
#include "resource.h"
#include "framework/misc/ZipFunction.h"

using namespace Gdiplus;

namespace OpenHMI
{
	CLogicManager	*CLogicManager::s_pInstance	= NULL;

	CLogicManager*	CLogicManager::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CLogicManager();
		}

		return s_pInstance;
	}

	void CLogicManager::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CLogicManager::CLogicManager()
		: m_iWallIdx(0)
		, m_bAdminOK(FALSE)
		, m_bLoginUdiskOK(FALSE)
	{
		;
	}

	CLogicManager::~CLogicManager()
	{
		;
	}

	// TODO: finish service event map here
	BEGIN_EVENT_MAP(CLogicManager)
		
	END_EVENT_MAP(CBaseLogic)

	// TODO: implement your public method here
	BOOLEAN CLogicManager::initialize()
	{
		AfxWriteLogger(g_Logger, TEXT("CLogicManager::initialize"));

		DWORD dwLanguageId = 0;
		DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
		if (ERROR_SUCCESS == ::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
			TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize) && dwRegType == REG_DWORD)
		{
			AfxSetLangId((LANGID)dwLanguageId);
		}

		if (!CBaseLogic::initialize())
		{
			return FALSE;
		}	

		m_up_load_gameclick.StartUploadGameClick();
		if (!m_xml.LoadIdcXml())
			m_xml.LoadDefautXml();


		String strSrcFile = OpenHMI::CPath::getApplicationFolder() + L"config\\newres.zip";
		String strDestPath = OpenHMI::CPath::getApplicationFolder() + L"config\\res";
		if (PathFileExists((const OpenHMI::CHAR*)strDestPath))
		{
			deleteDirectory((const OpenHMI::CHAR*)strDestPath);
		}

		WriteLog(TEXT("Start extract res.zip."));
		ExtractZipToDir(strSrcFile, strDestPath);
		WriteLog(TEXT("End extract res.zip."));
		CEvenyOneSD sd;
		m_hEventAll = CreateEvent(sd.GetSA(), FALSE, FALSE, _T("Global\\_pkg_refresh_data"));
		if (m_hEventAll == NULL)
			return FALSE;

		m_hEventExited = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hEventExited == NULL)
			return FALSE;

		m_hEventThread = (HANDLE)_beginthreadex(NULL, 0, ThreadEvent, this, 0, NULL);
		if (m_hEventThread == NULL)
			return FALSE;

		return TRUE;
	}

	void CLogicManager::unInitialize()
	{
		// TODO: add your un-initialize code here
		
		CloseHandle(m_hEventAll);
		CloseHandle(m_hEventExited);
		CloseHandle(m_hEventThread);
		CClientDbMgr::getInstance()->unInitialize();
		CBaseLogic::unInitialize();
	}

	void CLogicManager::buildWallPaper()
	{
		m_arySkinItem.clear();

		for (UINT uiIndex = 2; uiIndex < 4; ++uiIndex)
		{
			skinItem* pItem = new skinItem;

			TCHAR szTest[MAX_PATH] = { 0 };

			::LoadString(::GetModuleHandle(NULL), IDS_AYEL_NAME + uiIndex, szTest, MAX_PATH);

			int nIndex = SkinNameToType(szTest);

			pItem->name = szTest;
			pItem->path = String::format(L"%s%d.jpg", L"MenuSkin/WallpaperS/", nIndex - 1);
			pItem->type = 0;

			m_arySkinItem.addItem(pItem);
		}
		
		m_aryWallPaperPath.clear();

		for (UINT uiIndex = 0; uiIndex < 2; ++uiIndex)
		{
			String* pStrName = new String(String::format(L"%s%d.jpg", L"MenuSkin/Wallpaper/", uiIndex + 1));
			m_aryWallPaperPath.addItem(pStrName);
		}

		tstring strCustomBg = AfxGetDbMgr()->GetStrOpt(OPT_M_CUSTOMMENUBG);
		std::vector<tstring> strSplit;
		AfxSplitString(strCustomBg, strSplit, _T('|'));
		for (size_t i = 0; i < strSplit.size(); i++)
		{
			CString strFileName = strSplit[i].c_str();
			TCHAR szImage[MAX_PATH] = { 0 };

			AfxGetAppPath(szImage, MAX_PATH, _T("Skin\\Image\\"));
			lstrcat(szImage, strFileName);
			if (PathFileExists(szImage))
			{
				String* pStrName = new String(szImage);
				m_aryWallPaperPath.addItem(pStrName);

				TCHAR szImageS[MAX_PATH] = { 0 };
				AfxGetAppPath(szImageS, MAX_PATH, _T("skin\\customS\\"));
				lstrcat(szImageS, strFileName);

				if (!PathFileExists(szImageS))
				{
					Gdiplus::Bitmap *pBmp = Gdiplus::Bitmap::FromFile(szImage);
					Gdiplus::Bitmap* pShortCutBmp = CLogicManager::getInstance()->scaleBitmap(pBmp, 246, 136);
					String* pStrName = new String(CPath::getApplicationFolder() + L"skin\\customS\\" + strFileName.GetBuffer());
					AfxCreateDirectory((const CHAR*)(*pStrName));
					CLogicManager::getInstance()->saveAsImageFile(pShortCutBmp, (const CHAR*)(*pStrName), L"image/png");

					delete pShortCutBmp;
					delete pBmp;
				}

				skinItem* pItem = new skinItem;

				pItem->name = strFileName.GetBuffer();
				pItem->path = szImageS;
				pItem->type = 1;

				m_arySkinItem.addItem(pItem);
			}
		}

		skinItem* pItem = new skinItem;


		String* pStr = new String(CLanguage::getString(RES_LAG_ADD_SKIN));
		pItem->name = *pStr;
		pItem->path = L"";
		pItem->type = 2;

		m_arySkinItem.addItem(pItem);

	}

	void CLogicManager::OnClickCategroyList(int id, CString url, bool bFromDesk)
	{
		m_ClickCategroyList.m_id = id;
		m_ClickCategroyList.m_url = url;
		m_ClickCategroyList.m_bFromDesk = bFromDesk;

		dispatchEvent(CLICK_CATEGROY_LIST);
	}

	void CLogicManager::RunGame(bool bValue, DWORD gid, CString url, int type, std::string pos)
	{
		m_RunGame.gid = gid;
		m_RunGame.url = url;
		m_RunGame.type = type;
		m_RunGame.bFromGame = bValue;
		m_RunGame.pos = pos;

		dispatchEvent(RUN_GAME);
	}

	UINT __stdcall CLogicManager::ThreadEvent(LPVOID lpVoid)
	{
		CLogicManager* pFrame = reinterpret_cast<CLogicManager*>(lpVoid);
		HANDLE handle[] = { pFrame->m_hEventAll, pFrame->m_hEventExited };
		while (1)
		{
			DWORD  dwRet = WaitForMultipleObjects(2, handle, FALSE, 5000);
			if (dwRet == WAIT_OBJECT_0)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(pFrame->m_hEventExited, 5000))
					break;

				pFrame->dispatchEvent(REFRESH_DATA);
				pFrame->setRefreshData(0, 0);
			}
			else if (dwRet == WAIT_OBJECT_0 + 1)
			{
				break;
			}
			else if (dwRet == WAIT_ABANDONED_0)
			{
				break;
			}

			static bool bcliRunning = true, bOldRunning = true;
			bcliRunning = AfxGetDbMgr()->ClientServiceIsRunning();
			if (bOldRunning != bcliRunning)
			{
				if (bcliRunning)
				{				
					pFrame->setRefreshData(1, 1);
				}
				else
				{
					pFrame->setRefreshData(0, 1);
				}

				pFrame->dispatchEvent(REFRESH_DATA);

				bOldRunning = bcliRunning;
			}
		}
		return 0;
	}

	Gdiplus::Bitmap* CLogicManager::scaleBitmap(Gdiplus::Bitmap * pBitmap, UINT nWidth, UINT nHeight)
	{
		Gdiplus::Bitmap * pTemp = new Gdiplus::Bitmap(nWidth, nHeight, pBitmap->GetPixelFormat());
		if (pTemp)
		{
			Gdiplus::Graphics * g = Gdiplus::Graphics::FromImage(pTemp);
			if (g)
			{
				// use the best interpolation mode
				g->SetInterpolationMode(Gdiplus::InterpolationModeDefault);
				g->DrawImage(pBitmap, 0, 0, nWidth, nHeight);
				delete g;
			}
		}

		return pTemp;
	}

	bool CLogicManager::saveAsImageFile(Image* pImage, LPCWSTR lpszFileName, LPCWSTR lpszImageType)
	{
		UINT  num = 0;

		// number of image encoders
		UINT  size = 0;

		// size of the image encoder array in bytes
		ImageCodecInfo* pImageCodecInfo = NULL;

		if (GetImageEncodersSize(&num, &size) != Ok || size == 0)
			return false;  // Failure

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL)
			return false;  // Failure

		if (GetImageEncoders(num, size, pImageCodecInfo) != Ok)
			return false;

		bool bOk = false;
		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, lpszImageType) == 0)
			{
				pImage->Save(lpszFileName, &(pImageCodecInfo[j].Clsid));
				bOk = true;
				break;
			}

		}

		free(pImageCodecInfo);
		return bOk;

	}

	void CLogicManager::deleteDirectory(const String& szDirPath)
	{
		String szCplDirPath = szDirPath + L"\\";
		String szFindPath = szDirPath + L"\\*";
		WIN32_FIND_DATA fileData;

		HANDLE hFind = FindFirstFile((const CHAR*)szFindPath, &fileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			DeleteFile((const CHAR*)szDirPath);
			return;
		}

		// 跳过遍历出的"..//"
		FindNextFile(hFind, &fileData);

		while (FindNextFile(hFind, &fileData))
		{
			String szChildDirPath = szCplDirPath + fileData.cFileName;
			deleteDirectory(szChildDirPath);
		}

		FindClose(hFind);
		RemoveDirectory((const CHAR*)szCplDirPath);
	}
}
