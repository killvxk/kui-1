/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../Platform/ScreenTouch/IScreenTouchListener.h"

namespace OpenHMI
{
	class OPENHMI_API CScreenTouchManager : public IScreenTouchListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	CScreenTouchManager*	getInstance();
		static	void					deleteInstance();

		void	addCanvas(ICanvas *pCanvas);
		void	removeCanvas(ICanvas *pCanvas);

		void	removeCursorTracker(ICursorTracker* pCursorTracker);

		// implement interfaces
		virtual void	onCursorMoved(const CCursor &cursor);
		virtual void	onCursorPressDown(const CCursor &cursor);
		virtual void	onCursorPressDrag(const CCursor &cursor);
		virtual void	onCursorPressUp(const CCursor &cursor);
		virtual	void	onCursorPressDownR(const CCursor &cursor);
		virtual	void	onCursorPressUpR(const CCursor &cursor);
		virtual	void	onWheel(const CCursor &cursor) ;
		virtual void	onLostFocus();

	protected:// method
		CScreenTouchManager();
		CScreenTouchManager(const CScreenTouchManager&){}
		virtual ~CScreenTouchManager();		
		CScreenTouchManager& operator =(const CScreenTouchManager&){return *this;}

	private:// method

	protected:// property

	private:// property
		static	CScreenTouchManager	*s_pInstance;

		List<ICanvas>										m_listCanvas;
		// cursor id <--> cursor tracker
		Map<UINT, ICursorTracker, tagNumCompare<UINT> >		m_mapTracker;
	};
}