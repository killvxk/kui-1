#include "HMI.h"
#include "OSFDlg.h"
#include "resource.h"

namespace OpenHMI
{
	CFuncWgtSkinItem::CFuncWgtSkinItem()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_btnItem);
		addComponent(&m_btnDel);
		addComponent(&m_btnAdd);


		// TODO: add event listener to children controls here
		m_btnItem.addEventListener(this);
		m_btnDel.addEventListener(this);
		m_btnAdd.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtSkinItem::~CFuncWgtSkinItem()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtSkinItem)
		CONTROL_EVENT_HANDLER(&m_btnItem, CEventButton::TOUCH_PRESSED, onBtnItemTchPre)
		CONTROL_EVENT_HANDLER(&m_btnDel, CEventButton::TOUCH_RELEASED, onBtnDelTchPre)
		CONTROL_EVENT_HANDLER(&m_btnAdd, CEventButton::TOUCH_RELEASED, onBtnAddTchRel)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtSkinItem::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtSkinItem::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtSkinItem::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_SKIN_ITEM);
		INIT::initButton(m_btnItem, RES_ITM_BTN_ITEM, this);
		INIT::initButton(m_btnDel, RES_ITM_BTN_DEL, this);
		INIT::initButton(m_btnAdd, RES_ITM_BTN_ADD, this);


		// TODO: set data to children controls or set children controls status here
	}

	// TODO: implement your public method here

	void CFuncWgtSkinItem::onLostHover()
	{
		m_btnItem.onLostHover();
	}

	void CFuncWgtSkinItem::onMove(INT iX, INT iY)
	{
		m_btnItem.onMove(iX, iY);
		m_btnAdd.onMove(iX, iY);
		m_btnDel.onMove(iX, iY);
		//m_btnDel.littleRefresh();
	}

	// TODO: implement control event listener here
	void CFuncWgtSkinItem::onBtnItemTchPre(CComponent *pTarget)
	{
		CLogicManager::getInstance()->skinChange();
	}

	void CFuncWgtSkinItem::onBtnDelTchPre(CComponent *pTarget)
	{
		CLogicManager::getInstance()->skinDel();
	}

	void CFuncWgtSkinItem::onBtnAddTchRel(CComponent *pTarget)
	{
		HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadProc, this, 0, NULL));

		if (hThread != NULL)
		{
			::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
		}
	}


	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
	UINT __stdcall CFuncWgtSkinItem::threadProc(void *pParam)
	{
		//CFuncWgtSkinItem *pFuncWgtSkinItem = static_cast<CFuncWgtSkinItem*>(pParam);

		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;

		//LPCTSTR lpszFilter = _T("ËùÓÐÍ¼Æ¬ (*.jpg;*.png)\0*.jpg;*.png\0JPG (*.jpg)\0*.jpg\0PNG (*.png)\0*.png\0\0");
		LPCTSTR lpszFilter = _T("(*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0(*.jpg)\0*.jpg\0(*.bmp)\0*.bmp\0(*.png)\0*.png\0\0");

		//TCHAR szTest[MAX_PATH] = { 0 };

		//::LoadString(::GetModuleHandle(NULL), IDS_PIC, szTest, MAX_PATH);

		//lpszFilter = szTest;
		LPCTSTR lpszDefExt = _T("*.jpg\0*.png\0\0");

		static COSFDialog osfDlg(hWnd);

		if (osfDlg.FileOpenDlg((LPTSTR)lpszFilter, (LPTSTR)lpszDefExt, NULL, NULL, NULL, FALSE))
		{
			CString str = osfDlg.GetFileName();
			int nPos = str.ReverseFind(_T('\\'));
			CString strFileName = str.Right(str.GetLength() - nPos - 1);

			CString strBg = AfxGetDbMgr()->GetStrOpt(OPT_M_CUSTOMMENUBG);
			vector<tstring> strSplit;
			AfxSplitString(strBg.GetBuffer(), strSplit, _T('|'));
			if (strSplit.size() >= 10)
			{
				CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_SKINS_EXCEED_10), FALSE);
				return 0;
			}
			bool bExist = false;
			for (size_t i = 0; i < strSplit.size(); i++)
			{
				CString strOldFileName = strSplit[i].c_str();
				if (strOldFileName.CompareNoCase(strFileName) == 0)
				{
					bExist = true;
					break;
				}
			}

			TCHAR szDstPath[MAX_PATH] = { 0 };
			AfxGetAppPath(szDstPath, MAX_PATH, _T("Skin\\Image\\"));
			CString	strImagePath = szDstPath;
			strImagePath += strFileName;
			AfxCreateDirectory(strImagePath);

			Gdiplus::Bitmap *pBmp = Gdiplus::Bitmap::FromFile(str.GetBuffer());

			if (pBmp == NULL)
			{
				CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_FILE_OPEN_ERROR), FALSE);
				return 0;
			}

			bool bRet;

			if (pBmp->GetWidth() > 1920 && pBmp->GetHeight() > 1920)
			{
				Gdiplus::Bitmap* pDstBmp = CLogicManager::getInstance()->scaleBitmap(pBmp, 1920, 1920);

				bRet = CLogicManager::getInstance()->saveAsImageFile(pDstBmp, strImagePath, L"image/png");

				delete pDstBmp;
			}
			else
			{
				bRet = CopyFile(str, strImagePath, FALSE) == TRUE;
			}

			Gdiplus::Bitmap* pShortCutBmp = CLogicManager::getInstance()->scaleBitmap(pBmp, 246, 136);
			String* pStrName = new String(CPath::getApplicationFolder() + L"skin\\customS\\" + strFileName.GetBuffer());
			AfxCreateDirectory((const CHAR*)(*pStrName));
			CLogicManager::getInstance()->saveAsImageFile(pShortCutBmp, (const CHAR*)(*pStrName), L"image/png");

			delete pShortCutBmp;
			delete pBmp;



			if (str.CompareNoCase(strImagePath) == 0 || bRet || GetLastError() == 0)
			{
				if (!bExist)
				{
					if (strBg.IsEmpty())
						strBg = strFileName;
					else
					{
						strBg += _T("|");
						strBg += strFileName;
					}

					AfxGetDbMgr()->SetOpt(OPT_M_CUSTOMMENUBG, strBg);
					if (!AfxGetDbMgr()->SaveOpt())
					{
						CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_SAVE_OPTION_FAILED), FALSE);
						return 0;
					}

					CLogicManager::getInstance()->skinAdd();

					return 0;
				}
				else
				{
					CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_SKIN_EXIST), FALSE);
					return 0;
				}
			}
			else
			{
				DWORD nError = GetLastError();
				TCHAR szBufer[MAX_PATH] = { 0 };
				AfxGetWinErrorText(szBufer, MAX_PATH, nError);

				String str = String::format(L"%s:%u£¬%s", (const CHAR*)CLanguage::getString(RES_LAG_COPY_FILE_FAILED), nError, szBufer);

				CMainFrame::getInstance()->showTips(str, FALSE);
				return 0;
			}
		}

		return 0;
	}
}