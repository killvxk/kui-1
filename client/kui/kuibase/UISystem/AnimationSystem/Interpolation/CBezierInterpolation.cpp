#include "OpenHMI.h"

namespace OpenHMI
{
	CBezierInterpolation::CBezierInterpolation(FLOAT fTimePct1, FLOAT fAmtPct1, FLOAT fTimePct2, FLOAT fAmtPct2)
	{
		const FLOAT FN = 1.0f / 100;
		Array2<INT> aryTimePct;
		Array2<FLOAT> aryAmtRat;

		aryTimePct.clear();
		aryAmtRat.clear();		
		m_aryAmtRat.clear();
		
		for(INT i = 1; i <= 100; ++i)
		{			
			FLOAT fI = i * FN;									
			FLOAT fTime = 3*fTimePct1*fI*Math::pow(1 - fI, 2) + 3*fTimePct2*Math::pow(fI, 2)*(1 - fI) + Math::pow(fI, 3);
			FLOAT fAmt = 3*fAmtPct1*fI*Math::pow(1 - fI, 2) + 3*fAmtPct2*Math::pow(fI, 2)*(1 - fI) + Math::pow(fI, 3);

			aryTimePct.addItem(static_cast<INT>(fTime * 100));
			aryAmtRat.addItem(fAmt);
		};

		for(INT i = 0; i < 100; ++i)
		{
			INT j = 0;
			for(; j < (INT)aryTimePct.size(); ++j)
			{
				if( i <= aryTimePct[j])
				{
					m_aryAmtRat.addItem(aryAmtRat[j]);
					break;
				}
			}

			if(j >= (INT)aryAmtRat.size())
			{
				m_aryAmtRat.addItem(1.0f);
			}
		}
	}

	CBezierInterpolation::~CBezierInterpolation()
	{
		;
	}
	
	CBaseAmtData* CBezierInterpolation::getInterpolatedValue(LONG lTime)
	{
		if (m_pAmtValue1 == NULL || m_pAmtValue2 == NULL)
		{
			return NULL;
		}
		
		if (lTime <= m_lTime1)
		{
			return m_pAmtValue1->cloneData();
		}
		else if (lTime >= m_lTime2)
		{
			return m_pAmtValue2->cloneData();
		}
		else
		{
			FLOAT fTimePct = (FLOAT)(lTime - m_lTime1) / (m_lTime2 - m_lTime1);
			FLOAT fAmtPct = m_aryAmtRat[static_cast<UINT>(fTimePct * 100)];

			return m_pAmtValue1->add((m_pAmtValue2->subtract(m_pAmtValue1)->multiply(fAmtPct)));
		}
	}
}