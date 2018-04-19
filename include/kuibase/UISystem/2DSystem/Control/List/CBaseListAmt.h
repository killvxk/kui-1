/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseListAmt
		: public Object
		, public CAdapterAnimationListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CBaseListAmt();
		virtual ~CBaseListAmt();

		friend class CCtrlList;
		friend class CCtrlList_Simple;

		virtual void		play();
		virtual void		stop();
		virtual BOOLEAN		prepareAmtData(Object* pData);
		virtual BOOLEAN		prepareAmtDataExt(Object* pData);

		// implement animation listener function
		virtual	void		onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void		onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CBaseListAmt(const CBaseListAmt&){}
		CBaseListAmt& operator =(const CBaseListAmt&){return *this;}

	private:// method

	protected:// property
		CCtrlList*						m_pList					;
		CCtrlList_Simple*				m_pListSimple			;
		CAnimationPlayer				m_animationPlayer		;
		ObjectPtr<CBaseAnimation>		m_pAnimation			;

	private:// property
		
	};
}