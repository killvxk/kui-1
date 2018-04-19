/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModelProgressbar : public CModel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CCtrlProgressbar;

	public:// method
		CModelProgressbar();
		virtual ~CModelProgressbar();

		void	setProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep = 0);
		void	setPos(INT iPos);
		INT		getPos() const			{ return m_iPos; }
		INT		getMin() const			{ return m_iMin; }
		INT		getMax() const			{ return m_iMax; }
		INT		getRange()				{ return m_iMax - m_iMin + 1; }

	protected:// method
		CModelProgressbar(const CModelProgressbar&){}
		CModelProgressbar& operator =(const CModelProgressbar&){return *this;}

	private:// method
		FLOAT	getPosRatio() const;
		INT		getStep() const			{ return m_iStep; }

	protected:// property

	private:// property
		INT				m_iMin;
		INT				m_iMax;
		INT				m_iPos;
		INT				m_iStep;
	};
}