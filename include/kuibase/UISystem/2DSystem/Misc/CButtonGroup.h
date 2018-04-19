/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CButtonGroup
	*			This class will cause that only one button can be in selected state at a time.
	*/
	class OPENHMI_API CButtonGroup 
		: public Object
		, public IEventListener
		, public IEventListenerChain
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CButtonGroup();
		virtual ~CButtonGroup();

		DECLARE_EVENT_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void			add(CCtrlButton *pCtrlButton);
		void			remove(CCtrlButton *pCtrlButton);

		void			setSelected(CCtrlButton *pCtrlButton);
		CCtrlButton*	getSelected();
		void			removeSelected();
		
		void			clear();		

		virtual BOOLEAN	contains(Object *pTarget);

	protected:// method
		CButtonGroup(const CButtonGroup&){}
		CButtonGroup& operator =(const CButtonGroup&){return *this;}

	private:// method
		void onTouchReleased(CComponent *pTarget);

	protected:// property

	private:// property
		CCtrlButton				*m_pBtnSelected;
		Array2<CCtrlButton*>	m_aryCtrlButton;
	};
}