/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CTableAmtTouchShake
		: public CBaseTableAmt
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CTableAmtTouchShake();
		virtual ~CTableAmtTouchShake();

		friend class CCtrlTable;

		virtual BOOLEAN prepareAmtData(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CTableAmtTouchShake(const CTableAmtTouchShake&){}
		CTableAmtTouchShake& operator =(const CTableAmtTouchShake&){return *this;}

	private:// method

	protected:// property

	private:// property
		ObjectPtr<CComponent>	m_pComponent;
	};
}