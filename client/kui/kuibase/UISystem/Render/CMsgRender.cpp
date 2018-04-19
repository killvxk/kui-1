#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgRender::CMsgRender()
	{
		m_eType = CBaseMessage::MT_RENDER;
		m_uiID = m_eType;
	}

	CMsgRender::~CMsgRender()
	{
		;
	}

	void CMsgRender::handleMessage()
	{
		//return;

		static	INT		s_iFrameCount = 0;
		static	ULONG	s_ulTimeTotal = 0;
		static	ULONG	s_ulTimeStart = 0;
		if (CONFIG::getOutputPerformance())
		{
			s_ulTimeStart = CTime::getTickCount();
		}
		
		ENV::lockDirty();
		CRenderGroup* pRenderGroup = CBaseMainFrame::getInstance()->buildRenderTree();

		IRenderSession *pRenderSession = CRenderEngine::getInstance()->getRenderSession(1);
		if (pRenderSession != NULL)
		{
			pRenderSession->beginDraw(NULL);

			pRenderSession->drawGroup(pRenderGroup);

			pRenderSession->endDraw();
			ENV::incRenderCounter();
		}		

		CBaseMainFrame::getInstance()->breakUpRenderTree();
		ENV::unlockDirty();

		if (CONFIG::getOutputPerformance())
		{
			s_ulTimeTotal += CTime::getTickCount() - s_ulTimeStart;
			s_iFrameCount++;
			if (s_iFrameCount == 100)
			{
				CLog::info(String(MODULE_NAME), String::format(L"The time for rendering one frame --- %d millisecond.\n", s_ulTimeTotal / 100));
				s_iFrameCount = 0;
				s_ulTimeTotal = 0;
			}
		}

		ENV::clearDirty();
	}
}