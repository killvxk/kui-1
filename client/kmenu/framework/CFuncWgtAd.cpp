#include "HMI.h"
#include "UIX.h"

namespace OpenHMI
{
	CFuncWgtAd::CFuncWgtAd()
		//: m_iSW(0)
		//, m_iSH(0)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_imgBg);
		addComponent(&m_tableAd);


		// TODO: add event listener to children controls here
		m_tableAd.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_tableAd.setView(this);
		m_tableAd.setCanWheel(FALSE);

	}

	CFuncWgtAd::~CFuncWgtAd()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtAd)
		CONTROL_EVENT_HANDLER_CHAIN(&m_tableAd, CEventButton::TOUCH_RELEASED, onBtnTableAdItemTchRel)
		//CONTROL_EVENT_HANDLER(&m_tableAd, CEventTable::ITEM_HOVER_FOCUSED, onBtnTableAdItemHoverFocused)
		//CONTROL_EVENT_HANDLER(&m_tableAd, CEventTable::ITEM_HOVER_CANCELED, onBtnTableAdItemHoverCanceled)
		//CONTROL_EVENT_HANDLER(&m_tableAd, CEventTable::HOVER_CANCELED, onBtnTableAdItemHoverCanceled)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtAd::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		//CScreenTouchServer::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CFuncWgtAd::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.

		for (UINT i = 0; i < m_aryGifTimer.size(); i++)
		{
			m_aryGifTimer[i]->stop();
			m_aryGifTimer[i]->setListener(NULL);
		}
		m_aryGifTimer.clear();

		//CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtAd::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_AD);
		INIT::initImageField(m_imgBg, RES_ITM_IMG_BG, this);
		INIT::initTable(m_tableAd, RES_ITM_AD, this);


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
	CComponent* CFuncWgtAd::getItemComponent(CCtrlTable* pTable, Object* pObjItem)
	{
		INT iData = *((CInt*)pObjItem);
		CCtrlButton* pButton = new CCtrlButton();

		INIT::initButton(*pButton, RES_ITM_BTN_ITEM, this);
		CMovieClip* pMovieClip = new CMovieClip();			

		adItem* pItem = NULL;

		pItem = m_aryAdItem[iData];

		if (pItem == NULL)
			return NULL;

		String strFile = OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + pItem->item.fid.GetBuffer();
		if (CFile::isExist((const CHAR*)strFile) == TRUE)
		{
			if (pItem->bGif == TRUE)
			{
				CMovieClip::tagBitmapInfo info;

				info.strID = String::format(L"%s%d", pItem->item.fid.GetBuffer(), pItem->gif.animate_index);
				info.pImage = pItem->gif.animate_images[pItem->gif.animate_index].image;

				if (info.pImage != NULL)
				{
					pMovieClip->addImage(info);
				}
			}
			else
			{
				pMovieClip->addImage(OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + pItem->item.fid.GetBuffer());
			}

			pButton->getImageField(L"D")->setUserMovieClip(pMovieClip);
		}
		//m_iSW = 6;
		//m_iSH = 6;

		String strColor = L"0xFF";
		strColor = strColor + pItem->item.textcoloer.GetBuffer();

		int n;
		::StrToIntEx((const CHAR*)strColor, STIF_SUPPORT_HEX, &n);
		if (n >= 0xFF000000 && n <= 0xFFFFFFFF)
			pButton->getTextField()->setFontColor(n);

		String strName = pItem->item.text.GetBuffer();

		if (strName.length() > 8)
		{
			String s = strName.subString(0, 8);
			pButton->setCaption(s);
		}
		else
			pButton->setCaption(pItem->item.text.GetBuffer());

		CMovieClip* pMovieClipBg = new CMovieClip();
		pMovieClipBg->addImage(OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + pItem->item.backgroud.GetBuffer());
		pButton->getImageField(L"B")->setUserMovieClip(pMovieClipBg);
		pButton->setHandCursor(TRUE);
		pButton->addEventListener(this);
		pButton->refresh();

		return pButton;
	}

	void CFuncWgtAd::drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable)
	{
		return ;
	}

	void CFuncWgtAd::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{

	}

	void CFuncWgtAd::onTimerComplete(const CTimer *pTimer)
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
					info.strID = String::format(L"%s%d", pItem->item.fid.GetBuffer(), pItem->gif.animate_index);
					info.pImage = pItem->gif.animate_images[pItem->gif.animate_index].image;

					if (info.pImage != NULL)
					{
						pMovieClip->addImage(info);
						pButton->getImageField(L"D")->setUserMovieClip(pMovieClip);
					}
				}
			}
		}
	}

	// TODO: implement your public method here
	void CFuncWgtAd::changeSize(UINT w, UINT h)
	{
		this->setPreferredSize(w, h);
		m_tableAd.setPreferredSize(w, h);
		m_tableAd.resetTableLayout();
	}

	// TODO: implement control event listener here
	void CFuncWgtAd::onBtnTableAdItemTchRel(CComponent *pTarget)
	{
		INT i = m_tableAd.getCurTouchFocus();
		m_tableAd.setCurTouchFocus(-1);
		vector<ads> ad = CLogicManager::getInstance()->getMenuXml().GetAdbar();
		ads item;

		//onBtnTableAdItemHoverCanceled(NULL);

		item = m_aryAdItem[i]->item;
		//bool bResult = CLogicManager::getInstance()->getMenuXml().GetAdItem(ad[i].pos, item);
		//if (bResult)
		{
			std::stringstream stream;
			stream << "s" << i + 7;

			std::string strPos = stream.str();

			char szAid[10] = { 0 }, szGid[10];
			_ltoa_s(item.aid, szAid, 10);
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_AD, strPos, szAid);
			_ltoa_s(item.gid, szGid, 10);
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, strPos, szGid);

			if (item.action == 3  && !item.url.IsEmpty())
			{
				CLogicManager::getInstance()->OnClickCategroyList(item.gid, item.url);
			}
			else if (item.action == 2)
			{
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
#if 0
	void CFuncWgtAd::onBtnTableAdItemHoverFocused(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_tableAd.getHoverFocusedItem();

		if (pBtn != NULL && m_tableAd.getHoverFocus() == TRUE)
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"D");

			if (pImg == NULL)
				return;

			pImg->setX(pImg->getX() - m_iSW / 2);
			pImg->setY(pImg->getY() - m_iSH / 2);
			pImg->setPreferredSize(pImg->getPreferredSize().getX() + m_iSW, pImg->getPreferredSize().getY() + m_iSH);
		}
	}

	void CFuncWgtAd::onBtnTableAdItemHoverCanceled(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_tableAd.getHoverFocusedItem();

		if (pBtn != NULL && m_tableAd.getHoverFocus() == TRUE)
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"D");

			if (pImg == NULL)
				return;
			
			pImg->setX(pImg->getX() + m_iSW / 2);
			pImg->setY(pImg->getY() + m_iSH / 2);
	
			pImg->setPreferredSize(pImg->getPreferredSize().getX() - m_iSW, pImg->getPreferredSize().getY() - m_iSH);
		}
	}
#endif

	// TODO: implement logic event listener here
	void CFuncWgtAd::onSetFocus()
	{
		CTimerManager::getInstance()->start();
	}

	void CFuncWgtAd::onKillFocus()
	{
		CTimerManager::getInstance()->stop();
	}

	// TODO: implement your normal private method here
	void CFuncWgtAd::initAdItem()
	{
		m_aryAdItem.clear();
		vector<ads> ad = CLogicManager::getInstance()->getMenuXml().GetAdbar();

		for (int i = 0; i < (int)ad.size(); i++)
		{
			adItem* pItem = new adItem;

			pItem->item = ad[i];
			pItem->bGif = FALSE;
			pItem->gif.animate_index = 0;

			CString ext = GetFileExt(ad[i].fid.GetBuffer());
			ext.MakeLower();

			if (ext == _T("gif"))
			{
				unsigned char* buffer = NULL;
				unsigned int size = 0;
				String strFile = OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + ad[i].fid.GetBuffer();

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