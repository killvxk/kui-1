#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtNoResult::CFuncWgtNoResult()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_imgTips);
		addComponent(&m_tableGame);


		// TODO: add event listener to children controls here
		m_tableGame.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_tableGame.setView(this);
	}

	CFuncWgtNoResult::~CFuncWgtNoResult()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtNoResult)
		CONTROL_EVENT_HANDLER_CHAIN(&m_tableGame, CEventButton::TOUCH_RELEASED, onBtnTableItemTchRel)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtNoResult::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtNoResult::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtNoResult::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_NO_RESULT);
		INIT::initImageField(m_imgTips, RES_ITM_TIPS, this);
		INIT::initTable(m_tableGame, RES_ITM_TABLE_GAME, this);


		// TODO: set data to children controls or set children controls status here
		result r = CLogicManager::getInstance()->getMenuXml().GetResult();

		m_games.clear();


		for (unsigned int i = 0; i < r.arrGameRc.size(); i++)
		{
			m_games.push_back(r.arrGameRc[i]);
		}

		// ·ÅÈë¿Ø¼þ
		m_tableGame.getModel()->deleteAllItems();
		for (int i = 0; i < (int)m_games.size(); ++i)
		{
			m_tableGame.getModel()->addItem(new CInt(i));
		}

		if (m_games.size() > 0)
		{
			INT w = m_games.size() * m_tableGame.getCellWidth();
			INT tW = this->getPreferredSize().getX();

			m_tableGame.setX((tW - w) / 2);
			m_tableGame.setPreferredSize(w, m_tableGame.getPreferredSize().getY());
		}
	}

	// Definition functions for interface IViewTable
	CComponent* CFuncWgtNoResult::getItemComponent(CCtrlTable* pTable, Object* pObjItem)
	{
		if (pObjItem == NULL)
		{
			return NULL;
		}

		INT iData = *((CInt*)pObjItem);
		CCtrlButton* pButton = new CCtrlButton();

		INIT::initButton(*pButton, RES_ITM_BTN_ITEM, this);

		CMovieClip* pMovieClip = new CMovieClip();

		CMovieClip::tagBitmapInfo info;

		tagGameInfo* pGameInfo = NULL;
		customgame cust;
		CCtrlTextField* pText = pButton->getTextField(L"TXT");

		pGameInfo = m_games[iData];
		cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);

		if (pGameInfo->gid <= 100000)
		{
			info.strID = String::format(L"%d", pGameInfo->gid);
			info.pImage = GetGameIcon(pGameInfo->gid, 48);

			if (info.pImage != NULL)
			{
				pMovieClip->addImage(info);

			}
			else
			{
				BYTE* buffer = NULL;
				UINT size = 0;
				info.strID = L"MenuSkin/default.ico";

				if (AfxGetDbMgr()->m_defaultImage)
				{
					info.pImage = AfxGetDbMgr()->m_defaultImage;
				}
				else
				{
					BYTE* buffer = NULL;
					UINT size = 0;
					if (CZipSkin::getInstance()->LoadFileFromZipEx(L"MenuSkin/default.ico", &buffer, &size))
					{
						AfxGetDbMgr()->m_defaultImage = ReadIcon(buffer, size, 48);
						if (AfxGetDbMgr()->m_defaultImage)
						{
							info.pImage = AfxGetDbMgr()->m_defaultImage;
						}
					}
				}
				pMovieClip->addImage(info);
			}
			String str = pGameInfo->gname;
			if (str.length() >= 14)
			{
				str = str.subString(0, 13);
			}
			if (str.length() >= 7)
			{
				String s = str.subString(0, 6);
				CString szNew((const CHAR*)s);
				szNew.TrimRight();

				String strNew = szNew.GetBuffer();

				pText->setContent(strNew + L"\n" + str.subString(6));
			}
			else
			{
				pText->setContent(str);
			}
		}
		else
		{
			info.strID = cust.fid.GetBuffer();

			BYTE* buffer = NULL;
			UINT size = 0;
			if (CZipAd::getInstance()->LoadFileFromZipEx(cust.fid.GetBuffer(), &buffer, &size))
			{
				info.pImage = ReadIcon(buffer, size, 48);
			}

			if (info.pImage == NULL)
			{
				info.strID = L"MenuSkin/default.ico";

				if (AfxGetDbMgr()->m_defaultImage)
				{
					info.pImage = AfxGetDbMgr()->m_defaultImage;
				}
				else
				{
					BYTE* buffer = NULL;
					UINT size = 0;
					if (CZipSkin::getInstance()->LoadFileFromZipEx(L"MenuSkin/default.ico", &buffer, &size))
					{
						AfxGetDbMgr()->m_defaultImage = ReadIcon(buffer, size, 48);
						if (AfxGetDbMgr()->m_defaultImage)
						{
							info.pImage = AfxGetDbMgr()->m_defaultImage;
						}
					}
				}

			}
			pMovieClip->addImage(info);

			String str = cust.name.GetBuffer();
			if (str.length() >= 14)
			{
				str = str.subString(0, 13);
			}
			if (str.length() >= 7)
			{
				String s = str.subString(0, 6);
				CString szNew((const CHAR*)s);
				szNew.TrimRight();

				String strNew = szNew.GetBuffer();

				pButton->setCaption(strNew + L"\n" + str.subString(6));
			}
			else
			{
				pButton->setCaption(str);
			}
		}

		pButton->getImageField()->setUserMovieClip(pMovieClip);

		pButton->addEventListener(this);

		pButton->refresh();
		return pButton;
	}

	void CFuncWgtNoResult::drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable)
	{
		return ;
	}

	void CFuncWgtNoResult::changeSize(INT w, INT h)
	{
		INT w1 = m_games.size() * m_tableGame.getCellWidth();
		INT tW = w - getX();

		m_tableGame.setX((tW - w1) / 2);
		m_imgTips.setX((tW - m_imgTips.getPreferredSize().getX()) / 2);
	}

	// TODO: implement your public method here
	ICursorTracker* CFuncWgtNoResult::probe(INT iX, INT iY)
	{		
		if (getVisible() == FALSE)
			return NULL;

		return m_tableGame.probe(iX, iY);
	}

	// TODO: implement control event listener here
	void CFuncWgtNoResult::onBtnTableItemTchRel(CComponent *pTarget)
	{
		INT i = m_tableGame.getCurTouchFocus();
		m_tableGame.setCurTouchFocus(-1);

		tagGameInfo* pGame = m_games[i];
		std::stringstream stream;
		stream << "s" << i + 24;

		std::string strPos = stream.str();

		char szGid[10];
		_ltoa_s(pGame->gid, szGid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, strPos, szGid);

		CLogicManager::getInstance()->RunGame(false, pGame->gid, L"", 0, strPos);
	}

	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}