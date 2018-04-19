#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtPopUser::CFuncWgtPopUser()
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtUser);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtPopUser::~CFuncWgtPopUser()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtPopUser)

	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtPopUser::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtUser.initialize();


		return TRUE;
	}

	void CFuncWgtPopUser::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtUser.unInitialize();


	}

	void CFuncWgtPopUser::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_POP_USER);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtUser, RES_ITM_USER, this);


		// TODO: set data to children controls or set children controls status here
	}

	ICursorTracker* CFuncWgtPopUser::probe(INT iX, INT iY)
	{
		if (!getVisible())
		{
			return NULL;
		}

		if (m_funcWgtUser.hitTest(iX, iY))
		{
			return this;
		}
		m_funcWgtUser.quit();
		CLogicManager::getInstance()->leaveUser();

		return this;
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here


	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}