/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CTableAmtElasticity
		: public CBaseTableAmt
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CTableAmtElasticity();
		virtual ~CTableAmtElasticity();

		friend class CCtrlTable;

		virtual BOOLEAN prepareAmtData(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CTableAmtElasticity(const CTableAmtElasticity&){}
		CTableAmtElasticity& operator =(const CTableAmtElasticity&){return *this;}

	private:// method

	protected:// property

	private:// property
		INT		m_iStartValue;
		INT		m_iEndValue;
		INT		m_iStep;
	};
}