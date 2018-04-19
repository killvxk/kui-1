/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModelBalance : public CModel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CCtrlBalance;

	public:// method
		CModelBalance();
		virtual ~CModelBalance();

		void  setXProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep = 0);
		void  setXPos(INT iValue);
		INT   getXPos() const { return m_iXPos; }
		INT   getXMin() const { return m_iXMin; }
		INT   getXMax() const { return m_iXMax; }

		void  setYProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep = 0);
		void  setYPos(INT iValue);		
		INT   getYPos() const { return m_iYPos; }
		INT   getYMin() const { return m_iYMin; }		
		INT	  getYMax() const { return m_iYMax; }			

	protected:// method
		CModelBalance(const CModelBalance&){}
		CModelBalance& operator =(const CModelBalance&){return *this;}

	private:// method
		INT   getXRange();
		INT   getXStep() const { return m_iXStep; }
		FLOAT getXPosRatio();

		INT   getYRange();
		INT   getYStep() const { return m_iYStep; }
		FLOAT getYPosRatio();

	protected:// property

	private:// property
		INT		m_iXMin;
		INT		m_iXMax;
		INT		m_iXPos;
		INT		m_iXStep;

		INT		m_iYMin;		
		INT		m_iYMax;	
		INT		m_iYPos;		
		INT		m_iYStep;
	};
}