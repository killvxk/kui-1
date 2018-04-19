#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtHotSearch::CFuncWgtHotSearch()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_imgTag);
		addComponent(&m_imgBg);
		addComponent(&m_listMenu);
		addComponent(&m_imgFoot);


		// TODO: add event listener to children controls here
		m_listMenu.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_listMenu.setView(this);

	}

	CFuncWgtHotSearch::~CFuncWgtHotSearch()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtHotSearch)
		CONTROL_EVENT_HANDLER_CHAIN(&m_listMenu, CEventButton::TOUCH_RELEASED, onBtnListMenuItemTchRel)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtHotSearch::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener

		m_listMenu.setModel(&m_aryData);
		return TRUE;
	}

	void CFuncWgtHotSearch::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtHotSearch::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_HOT_SEARCH);
		INIT::initImageField(m_imgTag, RES_ITM_IMG_TAG, this);
		INIT::initImageField(m_imgBg, RES_ITM_IMG_BG, this);
		INIT::initList(m_listMenu, RES_ITM_MENU, this);
		INIT::initImageField(m_imgFoot, RES_ITM_IMG_FOOT, this);


		// TODO: set data to children controls or set children controls status here
		m_aryData.clear();
		m_listMenu.setReachToHead(TRUE);
		m_listMenu.setReachToTail(TRUE);
		vector<hotsearch> arSearch = CLogicManager::getInstance()->getMenuXml().GetHotSearch();
		for (size_t i = 0; i < arSearch.size(); i++)
		{
			m_aryData.addItem(new CInt(i));
		}
		CCtrlButton* pBtn = new CCtrlButton();

		INIT::initButton(*pBtn, RES_ITM_BTN_ITEM, this);

		m_imgBg.setPreferredSize(m_imgBg.getPreferredSize().getX(), m_aryData.size()*pBtn->getPreferredSize().getY());
		m_imgBg.setCanHitTest(TRUE);

		m_imgFoot.setY(m_imgBg.getY() + m_imgBg.getPreferredSize().getY());
		CHitAreaRectangle* pRect = new CHitAreaRectangle();
		pRect->setX(0);
		pRect->setY(0);
		pRect->setHeight(m_listMenu.getY() + m_aryData.size()*pBtn->getPreferredSize().getY());
		pRect->setWidth(getPreferredSize().getX());
		addHitTestArea(pRect);


		CHitAreaRectangle* pHTARect = new CHitAreaRectangle();
		pHTARect->setX(0);
		pHTARect->setY(0);
		pHTARect->setHeight( m_aryData.size()*pBtn->getPreferredSize().getY());
		pHTARect->setWidth(m_listMenu.getPreferredSize().getX());
		//m_listMenu.setPreferredSize(m_listMenu.getPreferredSize().getX(), m_aryData.size()*pBtn->getPreferredSize().getY());
		m_listMenu.addHitTestArea(pHTARect);
		delete pBtn;
	}

	// Definition functions for interface IViewList_Simple
	CComponent* CFuncWgtHotSearch::getItemComponent(CCtrlList_Simple* pList)
	{
		CComponent* pView = NULL;
		pView = new CCtrlButton();
		INIT::initButton(*((CCtrlButton*)pView), RES_ITM_BTN_ITEM, this);
		pView->addEventListener(this);
		pView->refresh();

		return pView;
	}

	void CFuncWgtHotSearch::drawItem(CCtrlList_Simple* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus)
	{
		CInt* pData = (CInt*)pObjItem;
		vector<hotsearch> arSearch = CLogicManager::getInstance()->getMenuXml().GetHotSearch();

		ASSERT(*pData >= 0 && *pData < (INT)arSearch.size());
		CCtrlButton *pBtn = (CCtrlButton*)pComponent;

		hotsearch s = arSearch[*pData];

		pBtn->setCaption(s.name.GetString());
		if (CFile::isExist(OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + s.icon.GetBuffer()) == TRUE)
		{
			CMovieClip* pMovieClip = new CMovieClip();

			pMovieClip->addImage(OpenHMI::CPath::getApplicationFolder() + L"config\\res\\" + s.icon.GetBuffer());
			pBtn->getImageField()->setUserMovieClip(pMovieClip);
		}

		return ;
	}


	// TODO: implement your public method here

	ICursorTracker* CFuncWgtHotSearch::probe(INT iX, INT iY)
	{
		if (!getVisible())
		{
			return NULL;
		}

		if (m_imgBg.hitTest(iX, iY))
		{
			return this;
		}
		
		setVisible(FALSE);
		return NULL;
	}

	// TODO: implement control event listener here
	void CFuncWgtHotSearch::onBtnListMenuItemTchRel(CComponent *pTarget)
	{
		INT i = m_listMenu.getCurTouchFocus();

		vector<hotsearch> arSearch = CLogicManager::getInstance()->getMenuXml().GetHotSearch();
		hotsearch s = arSearch[i];

		char szGid[10] = { 0 };
		_ltoa_s(s.gid, szGid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_AD, POS_MENUSEARDROP, szGid);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, POS_MENUSEARDROP, szGid);


		if (s.action == 3 && !s.url.IsEmpty())
		{
			CLogicManager::getInstance()->OnClickCategroyList(s.gid, s.url);
		}
		else if (s.action == 2)
		{
			CLogicManager::getInstance()->RunGame(false, s.gid, s.url, 2, POS_MENUSEARDROP);
		}
		else if (s.action == 1)
		{
			CLogicManager::getInstance()->RunGame(false, s.gid, s.url, 1, POS_MENUSEARDROP);
		}
		else
			CLogicManager::getInstance()->RunGame(false, s.gid, s.url, 0, POS_MENUSEARDROP);

		setVisible(FALSE);
	}

	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}