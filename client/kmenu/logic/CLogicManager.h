#pragma once

#include "OpenHMI.h"

namespace OpenHMI
{
	class CLogicManager : public CBaseLogic
	{
	public:// TODO: define const here
		static const  UINT UPDATE_WALL_PAPER			= 1;
		static const  UINT LEAVE_CHANGE_SKIN			= 2;
		static const  UINT LEAVE_USER					= 3;
		static const  UINT CLICK_CATEGROY_LIST			= 4;
		static const  UINT RUN_GAME						= 5;
		static const  UINT REFRESH_DATA					= 6;
		static const  UINT SKIN_CHANGE					= 7;
		static const  UINT SKIN_DEL						= 8;
		static const  UINT SKIN_ADD						= 9;
		static const  UINT SKIN_SAVE					= 10;
		static const  UINT LEAVE_ADMIN					= 11; 
		static const  UINT LEAVE_POPTIPS				= 12;
		static const  UINT LEAVE_POPUDISKLOGIN			= 13;
		static const  UINT LEAVE_POPUDISKREG			= 14;
		static const  UINT LEAVE_POPUDISK				= 15;
		static const  UINT LEAVE_CHANGE_PSW				= 16;
		static const  UINT LOGIN_POP_ADMIN_OK			= 17;
		static const  UINT UPLOAD_CFG					= 18;
		static const  UINT LEAVE_CFG					= 19;
		static const  UINT SAVE_FILE					= 20;
		static const  UINT READ_FILE					= 21;
		static const  UINT LEAVE_FILE					= 22;
		static const  UINT LOGIN_UDISK_OK				= 23;

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here
		class CClickCategroyList
		{
		public:
			CClickCategroyList()
				: m_id(0)
				, m_url(L"")
				, m_bFromDesk(FALSE)
			{

			}

			int m_id;
			CString m_url;
			bool m_bFromDesk;
		};

		class CRunGame
		{
		public:
			CRunGame()
				: bFromGame(FALSE)
				, gid(0)
				, url(L"")
				, type(0)
				, pos("")
			{

			}

			DWORD gid;
			CString url;
			int type;
			bool bFromGame;
			std::string pos;

		};

		struct  skinItem
		{
			INT	   type;    //0-系统自带 1-用户设置 2-ADD
			String name;
			String path;
		};

	private:// TODO: define embed class or struct or enum here

	public:// method
		static	CLogicManager*	getInstance();
		static	void			deleteInstance();

		DECLARE_EVENT_MAP
					
		virtual	BOOLEAN			initialize();
		virtual	void			unInitialize();
		void					buildWallPaper();
		INT						getWallPaperCount() const  { return m_arySkinItem.size(); }
		String&					getWallPaperName(INT iIndex) const { return m_arySkinItem[iIndex]->name; }
		String&					getWallPaperSPath(INT iIndex) const { return m_arySkinItem[iIndex]->path; }
		String&					getWallPaperPath(INT iIndex) const { return *(m_aryWallPaperPath[iIndex]); }
		INT						getWallPaperType(INT iIndex) const { return m_arySkinItem[iIndex]->type; }
		void					setCurWallPaperIdx(INT iIdx) { m_iWallIdx = iIdx; dispatchEvent(UPDATE_WALL_PAPER); }
		void					leaveChangeSkin() { dispatchEvent(LEAVE_CHANGE_SKIN); }
		void					leaveUser() { dispatchEvent(LEAVE_USER); }
		INT						getCurWallPaperIdx() const { return m_iWallIdx; }
		CIdcMenuXml&			getMenuXml()  { return m_xml; }
		CUpLoadGameClick&       getUpLoadGameClick() { return m_up_load_gameclick; }

		// TODO: define your public method here
		void					OnClickCategroyList(int id, CString url = L"", bool bFromDesk = false);
		void					RunGame(bool bValue, DWORD gid, CString url = _T(""), int type = 0, std::string pos = "");
		
		CClickCategroyList&     getClickCategroyList()  { return m_ClickCategroyList; }
		CRunGame&				getRunGame()  { return m_RunGame; }
		static UINT __stdcall	ThreadEvent(LPVOID lpVoid);

		void					setRefreshData(INT wP, INT lP) { m_iWP = wP; m_iLP = lP; }
		void					getRefreshData(INT& wP, INT& lP) { wP = m_iWP; lP = m_iLP; }

		void					skinChange() { dispatchEvent(SKIN_CHANGE); }
		void					skinDel() { dispatchEvent(SKIN_DEL); }
		void					skinAdd() { dispatchEvent(SKIN_ADD); }

		void					skinSave() { dispatchEvent(SKIN_SAVE); }
		void					leaveAdmin(BOOLEAN bAdminOK) { m_bAdminOK = bAdminOK;  dispatchEvent(LEAVE_ADMIN); }
		BOOLEAN					getAdminOK() { return m_bAdminOK; }

		BOOLEAN					getLoginUdiskOK() { return m_bLoginUdiskOK; }

		void					leavePopTips() { dispatchEvent(LEAVE_POPTIPS); }
		void					leavePopUdiskLogin(BOOLEAN bLoginUdiskOK) { m_bLoginUdiskOK = bLoginUdiskOK; dispatchEvent(LEAVE_POPUDISKLOGIN); }
		void					leavePopUdiskReg() { dispatchEvent(LEAVE_POPUDISKREG); }
		void					leavePopUdisk() { dispatchEvent(LEAVE_POPUDISK); }
		void					leavePopChangePsw() { dispatchEvent(LEAVE_CHANGE_PSW); }


		void					loginUdiskOK() { dispatchEvent(LOGIN_UDISK_OK); }
		void					loginPopAdminOK() { dispatchEvent(LOGIN_POP_ADMIN_OK); }
		void					uploadCfg() { dispatchEvent(UPLOAD_CFG); }
		void					leaveCfg() { dispatchEvent(LEAVE_CFG); }

		void					saveFile() { dispatchEvent(SAVE_FILE); }
		void					leaveFile() { dispatchEvent(LEAVE_FILE); }

		void					readFile() { dispatchEvent(READ_FILE); }

		bool					saveAsImageFile(Gdiplus::Image* pImage, LPCWSTR lpszFileName, LPCWSTR lpszImageType);
		Gdiplus::Bitmap* 		scaleBitmap(Gdiplus::Bitmap * pBitmap, UINT nWidth, UINT nHeight);

		void					deleteDirectory(const String& szDirPath);

	protected:// method
		CLogicManager();
		virtual ~CLogicManager();

	private:// method	
	
	protected:// property	

	private:// property
		static	CLogicManager		*s_pInstance;

		// TODO: define private property here
		Array<String>				m_aryIco;
		Array<String>				m_aryWallPaperPath;
		Array<skinItem>				m_arySkinItem;

		INT							m_iWallIdx;
		CIdcMenuXml					m_xml;
		CUpLoadGameClick			m_up_load_gameclick;
		CClickCategroyList			m_ClickCategroyList;
		CRunGame					m_RunGame;
		CString						m_strCmdLine;
		HANDLE 						m_hEventThread;
		HANDLE 						m_hEventAll;
		HANDLE 						m_hEventExited;

		INT							m_iWP;
		INT							m_iLP;

		BOOLEAN						m_bAdminOK;
		BOOLEAN						m_bLoginUdiskOK;
	};

}
