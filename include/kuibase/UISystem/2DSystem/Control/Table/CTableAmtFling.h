/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CTableAmtFling
		: public CBaseTableAmt
	{
	public:// const define		

	private:// const define
		static const INT FLING_START_SPEED	 = 100;
		static const INT FLING_DURATION_TIME = 100;

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CTableAmtFling();
		virtual ~CTableAmtFling();

		friend class CCtrlTable;

		virtual BOOLEAN prepareAmtData(Object* pData);
		virtual BOOLEAN prepareAmtDataExt(Object* pData);

		// implement animation listener function
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CTableAmtFling(const CTableAmtFling&){}
		CTableAmtFling& operator =(const CTableAmtFling&){return *this;}

	private:// method

	protected:// property

	private:// property
		INT		m_iStartValue;
		FLOAT	m_fFlingSpeed;

		INT		m_iStartPos;
		INT		m_iDeltaPos;
	};
}