/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CTableAmtEnterInterdigitation
		: public CBaseTableAmt
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CTableAmtEnterInterdigitation();
		virtual ~CTableAmtEnterInterdigitation();

		friend class CCtrlTable;

		virtual BOOLEAN prepareAmtData(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CTableAmtEnterInterdigitation(const CTableAmtEnterInterdigitation&){}
		CTableAmtEnterInterdigitation& operator =(const CTableAmtEnterInterdigitation&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}