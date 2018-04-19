/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CListAmtElasticity
		: public CBaseListAmt
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CListAmtElasticity();
		virtual ~CListAmtElasticity();

		friend class CCtrlList;
		friend class CCtrlList_Simple;

		virtual BOOLEAN prepareAmtData(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CListAmtElasticity(const CListAmtElasticity&){}
		CListAmtElasticity& operator =(const CListAmtElasticity&){return *this;}

	private:// method

	protected:// property

	private:// property
		INT		m_iStartValue;
		INT		m_iEndValue;
		INT		m_iStep;
	};
}