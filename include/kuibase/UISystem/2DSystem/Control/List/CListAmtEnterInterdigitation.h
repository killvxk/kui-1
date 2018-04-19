/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CListAmtEnterInterdigitation
		: public CBaseListAmt
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CListAmtEnterInterdigitation();
		virtual ~CListAmtEnterInterdigitation();

		friend class CCtrlList;
		friend class CCtrlList_Simple;

		virtual BOOLEAN prepareAmtData(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CListAmtEnterInterdigitation(const CListAmtEnterInterdigitation&){}
		CListAmtEnterInterdigitation& operator =(const CListAmtEnterInterdigitation&){return *this;}

	private:// method

	protected:// property

	private:// property
		INT				m_iActiveIndex;
		BOOLEAN			m_bInitialized;
	};
}