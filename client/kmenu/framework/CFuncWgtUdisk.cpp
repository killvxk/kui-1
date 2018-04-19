#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtUdisk::CFuncWgtUdisk()
		: m_pPopChangePsw(NULL)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txtTitle);
		addComponent(&m_btnQuit);
		addComponent(&m_btnOpenDisk);
		addComponent(&m_btnQuitDisk);
		addComponent(&m_txtUserTitle);
		addComponent(&m_txtDiskSizeTitile);
		addComponent(&m_txtUser);
		addComponent(&m_txtDiskSize);
		addComponent(&m_txtDiskUsedTitile);
		addComponent(&m_txtDiskUseTitile);
		addComponent(&m_txtDiskUsed);
		addComponent(&m_txtDiskUse);
		addComponent(&m_btnChangePsw);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnOpenDisk.addEventListener(this);
		m_btnQuitDisk.addEventListener(this);
		m_btnChangePsw.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtUdisk::~CFuncWgtUdisk()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtUdisk)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnOpenDisk, CEventButton::TOUCH_RELEASED, onBtnOpenDiskTchRel)
		CONTROL_EVENT_HANDLER(&m_btnQuitDisk, CEventButton::TOUCH_RELEASED, onBtnQuitDiskTchRel)
		CONTROL_EVENT_HANDLER(&m_btnChangePsw, CEventButton::TOUCH_RELEASED, onBtnChangePswTchRel)
		POPUP_EVENT_HANDLER(m_pPopChangePsw, CEventPopup::DESTROYED, onPopChangePsw)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtUdisk::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener

		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CFuncWgtUdisk::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.

		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtUdisk::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_UDISK);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnOpenDisk, RES_ITM_BTN_OPEN_DISK, this);
		INIT::initButton(m_btnQuitDisk, RES_ITM_BTN_QUIT_DISK, this);
		INIT::initTextField(m_txtUserTitle, RES_ITM_TXT_USER_TITLE, this);
		INIT::initTextField(m_txtDiskSizeTitile, RES_ITM_TXT_DISK_SIZE_TITILE, this);
		INIT::initTextField(m_txtUser, RES_ITM_TXT_USER, this);
		INIT::initTextField(m_txtDiskSize, RES_ITM_TXT_DISK_SIZE, this);
		INIT::initTextField(m_txtDiskUsedTitile, RES_ITM_TXT_DISK_USED_TITILE, this);
		INIT::initTextField(m_txtDiskUseTitile, RES_ITM_TXT_DISK_USE_TITILE, this);
		INIT::initTextField(m_txtDiskUsed, RES_ITM_TXT_DISK_USED, this);
		INIT::initTextField(m_txtDiskUse, RES_ITM_TXT_DISK_USE, this);
		INIT::initButton(m_btnChangePsw, RES_ITM_BTN_CHANGE_PSW, this);


		// TODO: set data to children controls or set children controls status here
		CString drv;
		drv.Format(TEXT("%c:\\"), AfxGetDbMgr()->GetUDiskUnmoutDriver());
		drv += TEXT("\\");
		ULARGE_INTEGER liSizeFree, liTotalSize;
		liSizeFree.QuadPart = 0;
		liTotalSize.QuadPart = 0;

		GetDiskFreeSpaceEx(drv, NULL, &liTotalSize, &liSizeFree);

		CString strUDiskName, strUDiskPassWord;

		GetPassword(strUDiskName, strUDiskName);
		m_txtUser.setContent(strUDiskName.GetBuffer());


		CString txt;
		txt.Format(_T("%0.2fM"), ((float)liTotalSize.QuadPart / (float)(1024 * 1024)));
		m_txtDiskSize.setContent(txt.GetBuffer());

		txt.Format(_T("%0.2fM"), ((float)(liTotalSize.QuadPart - liSizeFree.QuadPart) / (float)(1024 * 1024)));
		m_txtDiskUsed.setContent(txt.GetBuffer());

		txt.Format(_T("%0.2fM"), ((float)liSizeFree.QuadPart / (float)(1024 * 1024)));
		m_txtDiskUse.setContent(txt.GetBuffer());

	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtUdisk::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopUdisk();
	}

	void CFuncWgtUdisk::onBtnOpenDiskTchRel(CComponent *pTarget)
	{
		CString drv;
		drv.Format(TEXT("%c:\\"), AfxGetDbMgr()->GetUDiskUnmoutDriver());

		HINSTANCE ret = ShellExecute(NULL, TEXT("open"), drv, NULL, NULL, SW_SHOWNORMAL);
		if ((DWORD)ret > 32)
		{
			int j = 0;
		}
	}

	void CFuncWgtUdisk::onBtnQuitDiskTchRel(CComponent *pTarget)
	{
		CServer server;
		if (server.UDiskUnMount())
		{
		}

		SavePassword(L"", L"");
		CLogicManager::getInstance()->leavePopUdisk();
	}

	void CFuncWgtUdisk::onBtnChangePswTchRel(CComponent *pTarget)
	{
		if (m_pPopChangePsw == NULL)
		{
			CScreenTouchServer::getInstance()->removeEventListener(this);
			m_pPopChangePsw = new CPopChangePsw(FALSE);
			if (m_pPopChangePsw->initialize())
			{
				m_pPopChangePsw->addEventListener(this);
				m_pPopChangePsw->doModal();
			}
			else
			{
				delete m_pPopChangePsw;
				m_pPopChangePsw = NULL;
			}
		};
	}

	void CFuncWgtUdisk::onPopChangePsw(CBasePopup *pTarget)
	{
		m_pPopChangePsw->quit();
		m_pPopChangePsw = NULL;

		CScreenTouchServer::getInstance()->addEventListener(this);
	}

	// TODO: implement logic event listener here
	void CFuncWgtUdisk::onKeyEsc()
	{
		onBtnQuitTchRel(NULL);
	}
	// TODO: implement your normal private method here
}