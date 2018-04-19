/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModelScrollbar : public CModel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CCtrlScrollbar;

	public:// method
		CModelScrollbar();
		virtual ~CModelScrollbar();

		void	setScrollInfo(INT iMin, INT iMax, INT iPos, INT iPage);
		void	setRange(INT iMin, INT iMax);
		void	setPosition(INT iPos);
		void	setPreviousStep(INT iValue);
		void	setNextStep(INT iValue);

		INT		getMinimum();
		INT		getMaximum();
		INT		getPage();
		INT		getRange();
		INT		getPosition();
		INT		getMaxPosition();

	protected:// method
		CModelScrollbar(const CModelScrollbar&){}
		CModelScrollbar& operator =(const CModelScrollbar&){return *this;}

	private:// method
		FLOAT	getPosRatio();
		INT     moveStepPrevious();
		INT     moveStepNext();
		INT     movePagePrevious();
		INT     movePageNext();

	protected:// property

	private:// property
		INT m_iMin;
		INT m_iMax;
		INT m_iPage;
		INT	m_iPreviousStep;
		INT	m_iNextStep;
		INT m_iPos;
	};
}