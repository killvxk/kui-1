#include "HMI.h"
#include "UIX.h"

namespace OpenHMI
{
	CFuncWgtAdRec::CFuncWgtAdRec()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_imgTips);
		addComponent(&m_tableAd);
		//addComponent(&m_btnChange);


		// TODO: add event listener to children controls here
		m_tableAd.addEventListener(this);
		m_btnChange.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_tableAd.setView(this);
		m_tableAd.setCanWheel(FALSE);
	}

	CFuncWgtAdRec::~CFuncWgtAdRec()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtAdRec)
		CONTROL_EVENT_HANDLER(&m_btnChange, CEventButton::HOVER_FOCUSED, onBtnChangeHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnChange, CEventButton::HOVER_CANCELED, onBtnChangeHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_btnChange, CEventButton::TOUCH_RELEASED, onBtnChangeTouchRel)

		CONTROL_EVENT_HANDLER_CHAIN(&m_tableAd, CEventButton::TOUCH_RELEASED, onBtnTableAdItemTchRel)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtAdRec::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtAdRec::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		for (UINT i = 0; i < m_aryGifTimer.size(); i++)
		{
			m_aryGifTimer[i]->stop();
			m_aryGifTimer[i]->setListener(NULL);
		}
		m_aryGifTimer.clear();

	}

	void CFuncWgtAdRec::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_AD_REC);
		INIT::initImageField(m_imgTips, RES_ITM_TIPS, this);
		INIT::initTable(m_tableAd, RES_ITM_AD, this);
		INIT::initButton(m_btnChange, RES_ITM_BTN_CHANGE, this);


		// TODO: set data to children controls or set children controls status here
		initAdItem();

		m_tableAd.getModel()->deleteAllItems();

		for (UINT i = 0; i < m_aryGifTimer.size(); i++)
		{
			m_aryGifTimer[i]->stop();
			m_aryGifTimer[i]->setListener(NULL);
		}
		m_aryGifTimer.clear();

		m_tableAd.getModel()->deleteAllItems();

		for (int i = 0; i < (int)m_aryAdItem.size(); ++i)
		{
			m_tableAd.getModel()->addItem(new CInt(i));
		}

		for (INT i = 0; i < (int)m_aryAdItem.size(); i++)
		{
			adItem* pItem = m_aryAdItem[i];

			if (pItem->bGif == TRUE)
			{
				CTimer *pTimer = new CTimer;

				pTimer->setListener(this);
				pTimer->setDelay(pItem->gif.animate_images[pItem->gif.animate_index].delay);
				pTimer->setRepeatCount(1);
				pTimer->start();

				m_aryGifTimer.addItem(pTimer);
			}
			else
			{
				CTimer *pTimer = new CTimer;
				m_aryGifTimer.addItem(pTimer);
			}
		}

	}

	// Definition functions for interface IViewTable
	CComponent* CFuncWgtAdRec::getItemComponent(CCtrlTable* pTable, Object* pObjItem)
	{
		INT iData = *((CInt*)pObjItem);
		CCtrlButton* pButton = new CCtrlButton();

		INIT::initButton(*pButton, RES_ITM_BTN_ITEM, this);
		CMovieClip* pMovieClip = new CMovieClip();

		adItem* pItem = NULL;

		pItem = m_aryAdItem[iData];

		if (pItem == NULL)
			return NULL;

		String strFile = OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + pItem->item.file.GetBuffer();

		if (CFile::isExist((const CHAR*)strFile) == TRUE)
		{
			if (pItem->bGif == TRUE)
			{
				CMovieClip::tagBitmapInfo info;

				info.strID = String::format(L"%s%d", pItem->item.file.GetBuffer(), pItem->gif.animate_index);
				info.pImage = pItem->gif.animate_images[pItem->gif.animate_index].image;

				if (info.pImage != NULL)
				{
					pMovieClip->addImage(info);
				}
			}
			else
			{
				pMovieClip->addImage(OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + pItem->item.file.GetBuffer());
			}

			pButton->getImageField()->setUserMovieClip(pMovieClip);
		}

		pButton->setHandCursor(TRUE);
		pButton->addEventListener(this);
		pButton->refresh();

		return pButton;
	}

	void CFuncWgtAdRec::drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable)
	{
		return ;
	}
	void CFuncWgtAdRec::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{

	}

	void CFuncWgtAdRec::onTimerComplete(const CTimer *pTimer)
	{
		for (UINT i = 0; i < m_tableAd.getModel()->getItemCount(); i++)
		{
			CCtrlButton *pButton = (CCtrlButton *)m_tableAd.getItemComponent(m_tableAd.getModel()->getItemAt(i));
			adItem* pItem = m_aryAdItem[i];

			if (pItem != NULL)
			{
				if (pTimer == m_aryGifTimer[i])
				{
					pItem->gif.animate_index++;
					if (pItem->gif.animate_index >= (INT)pItem->gif.animate_images.size())
					{
						pItem->gif.animate_index = 0;
					}

					m_aryGifTimer[i]->stop();
					m_aryGifTimer[i]->setDelay(pItem->gif.animate_images[pItem->gif.animate_index].delay);
					m_aryGifTimer[i]->setRepeatCount(1);
					m_aryGifTimer[i]->start();

					CMovieClip::tagBitmapInfo info;
					CMovieClip* pMovieClip = new CMovieClip();
					info.strID = String::format(L"%s%d", pItem->item.file.GetBuffer(), pItem->gif.animate_index);
					info.pImage = pItem->gif.animate_images[pItem->gif.animate_index].image;

					if (info.pImage != NULL)
					{
						pMovieClip->addImage(info);
						pButton->getImageField()->setUserMovieClip(pMovieClip);
					}
				}
			}
		}
	}

	// TODO: implement your public method here
	void CFuncWgtAdRec::changeSize(UINT w, UINT h)
	{
		this->setPreferredSize(w, h);
		m_tableAd.setPreferredSize(w, m_tableAd.getPreferredSize().getY());
		m_tableAd.resetTableLayout();
	}

	// TODO: implement control event listener here
	void CFuncWgtAdRec::onBtnChangeHoverFocused(CComponent *pTarget)
	{
		m_btnChange.getImageField(L"IMG")->showImage(2);
	}

	void CFuncWgtAdRec::onBtnChangeHoverCanceled(CComponent *pTarget)
	{
		m_btnChange.getImageField(L"IMG")->showImage(1);
	}

	void CFuncWgtAdRec::onBtnChangeTouchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->OnClickCategroyList(CLogicManager::getInstance()->getMenuXml().GetResult().defSubID);
	}

	// TODO: implement control event listener here
	void CFuncWgtAdRec::onBtnTableAdItemTchRel(CComponent *pTarget)
	{
		INT i = m_tableAd.getCurTouchFocus();
		m_tableAd.setCurTouchFocus(-1);
		vector<adRc> ad = CLogicManager::getInstance()->getMenuXml().GetResult().arrAdRc;
		adRc item;

		item = m_aryAdItem[i]->item;
		
		{
			std::stringstream stream;
			stream << "s" << i + 30;

			std::string strPos = stream.str();

			char szAid[10] = { 0 }, szGid[10];
			_ltoa_s(item.aid, szAid, 10);
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_AD, strPos, szAid);
			_ltoa_s(item.gid, szGid, 10);
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, strPos, szGid);

			if (item.action == 3 && !item.url.IsEmpty())
			{
				CLogicManager::getInstance()->OnClickCategroyList(item.gid, item.url);
			}
			else if (item.action == 2)
			{
				tagGameInfo* pGameInfo = AfxGetDbMgr()->FindGameInfo(item.gid);
				if (pGameInfo == NULL)
				{
					CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_GAME_NOT_DOWNLOAD));
					return;

				}
				else
				{
					if (pGameInfo->runtype == GAME_RUNTYPE_UPDATE)
					{
						if (!PathFileExists(pGameInfo->gclipath))
						{
							String str = String::format(L"%s\n %s.", pGameInfo->gname, (const CHAR*)CLanguage::getString(RES_LAG_FILE_NOT_EXIST));

							CMainFrame::getInstance()->showTips(str);
							return;
						}

					}

				}
				CLogicManager::getInstance()->RunGame(false, item.gid, item.url, 2, strPos);
			}
			else if (item.action == 1)
			{
				CLogicManager::getInstance()->RunGame(false, item.gid, item.url, 1, strPos);
			}
			else
				CLogicManager::getInstance()->RunGame(false, item.gid, item.url, 0, strPos);
		}
	}

	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
	void CFuncWgtAdRec::initAdItem()
	{
		m_aryAdItem.clear();
		vector<adRc> ad = CLogicManager::getInstance()->getMenuXml().GetResult().arrAdRc;

		for (int i = 0; i < (int)ad.size(); i++)
		{
			adItem* pItem = new adItem;

			pItem->item = ad[i];
			pItem->bGif = FALSE;
			pItem->gif.animate_index = 0;

			CString ext = GetFileExt(ad[i].file.GetBuffer());
			ext.MakeLower();

			if (ext == _T("gif"))
			{
				unsigned char* buffer = NULL;
				unsigned int size = 0;
				String strFile = OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + ad[i].file.GetBuffer();

				if (!ReadFileData((const CHAR*)strFile, buffer, size))
				{
					m_aryAdItem.addItem(pItem);
					continue;
				}

				if (!LoadGif(&pItem->gif.animate_images, buffer, size))
				{
					m_aryAdItem.addItem(pItem);
					free(buffer);
					continue;
				}

				free(buffer);

				pItem->bGif = TRUE;
			}
			m_aryAdItem.addItem(pItem);
		}
	}
}