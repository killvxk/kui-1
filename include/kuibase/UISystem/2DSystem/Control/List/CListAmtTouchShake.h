/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CListAmtTouchShake
		: public CBaseListAmt
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CListAmtTouchShake();
		virtual ~CListAmtTouchShake();

		friend class CCtrlList;
		friend class CCtrlList_Simple;

		virtual BOOLEAN prepareAmtData(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CListAmtTouchShake(const CListAmtTouchShake&){}
		CListAmtTouchShake& operator =(const CListAmtTouchShake&){return *this;}

	private:// method

	protected:// property

	private:// property
		ObjectPtr<CComponent>	m_pComponent;
	};
}