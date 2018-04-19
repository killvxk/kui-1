/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseTableAmt
		: public Object
		, public CAdapterAnimationListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CBaseTableAmt();
		virtual ~CBaseTableAmt();

		friend class CCtrlTable;

		virtual void		play();
		virtual void		stop();
		virtual BOOLEAN		prepareAmtData(Object* pData);
		virtual BOOLEAN		prepareAmtDataExt(Object* pData);

		// implement animation listener function
		virtual	void		onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void		onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CBaseTableAmt(const CBaseTableAmt&){}
		CBaseTableAmt& operator =(const CBaseTableAmt&){return *this;}

	private:// method

	protected:// property
		CCtrlTable*						m_pTable				;
		CAnimationPlayer				m_animationPlayer		;
		ObjectPtr<CBaseAnimation>		m_pAnimation			;

	private:// property
		
	};
}