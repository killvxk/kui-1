/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderGroup : public CRenderItem
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ERotateAxis
		{
			RA_X	= 0,
			RA_Y,
			RA_Z,
		};

	private:// embed class or struct or enum		

	public:// method		
		CRenderGroup(BOOLEAN bManageLifeCycle = FALSE);
		virtual ~CRenderGroup();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		BOOLEAN						getNeedCache() const					{ return m_bNeedCache; }
		void						setNeedCache(BOOLEAN bValue)			{ m_bNeedCache = bValue; }

		BOOLEAN						getNeedClip() const						{ return m_bNeedClip; }
		void						setNeedClip(BOOLEAN bValue)				{ m_bNeedClip = bValue; }
		
		BOOLEAN						getHasViewport() const					{ return m_bHasViewport; }
		void						setHasViewport(BOOLEAN bValue)			{ m_bHasViewport = bValue; }		

		BOOLEAN						getFrequentChangeFlag() const			{ return m_bFrequentChangeFlag; }
		void						setFrequentChangeFlag(BOOLEAN bValue)	{ m_bFrequentChangeFlag = bValue; }

		BOOLEAN						getCanMakeCache() const					{ return m_bCanMakeCache;			}
		void						setCanMakeCache(BOOLEAN bValue)			{ m_bCanMakeCache = bValue;			}

		BOOLEAN						isRenderTextWrapper();
		BOOLEAN						isRenderImageWrapper();		
		BOOLEAN						isRenderViewportWrapper();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void						setAlpha(UINT uiValue);
		UINT						getAlpha()								{ return m_uiAlpha; }

		FLOAT						getRotateRadian() const					{ return m_fRotateRadian; }
		ERotateAxis					getRotateAxis()	const					{ return m_eRA; }
		INT							getAnchorPointX() const					{ return m_iAnchorX; }
		INT							getAnchorPointY() const					{ return m_iAnchorY; }
		void						setRotateRadian(FLOAT fValue, INT iAnchorX, INT iAnchorY, ERotateAxis eRA);

		FLOAT						getScaleWidth() const					{ return m_fScaleWidth; }
		void						setScaleWidth(FLOAT fValue);

		FLOAT						getScaleHeight() const					{ return m_fScaleHeight; }
		void						setScaleHeight(FLOAT fValue);
	
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		UINT						getFilterCount() const					{ return m_aryFilter.size(); }
		CRenderBaseFilter*			getFilterAt(UINT uiIndex)				{ return m_aryFilter[uiIndex]; }
		void						clearAllFilter();
		void						addFilter(CRenderBaseFilter *pFilter);
		void						updateFilter();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		UINT						getRenderItemCount() const				{ return m_aryChildren.size(); }
		CRenderItem*				getRenderItemAt(UINT uiIndex)			{ return m_aryChildren[uiIndex]; }
		void						addRenderItem(CRenderItem *pRenderItem);
		void						insertRenderItem(UINT uiIndex, CRenderItem *pRenderItem);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void						destroy();
		void						breakUp();
		void						checkChildrenChanged();
		void						updateCacheFlagStep1();
		void						updateCacheFlagStep2();

		virtual CRenderItem*		hitTest(INT iX, INT iY);

	protected:// method
		CRenderGroup(const CRenderGroup&){}
		CRenderGroup& operator =(const CRenderGroup&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		Array<CRenderItem>		m_aryChildren;
		Array2<CRenderItem*>	m_aryBackupChildren;

		BOOLEAN					m_bNeedCache;		
		BOOLEAN					m_bNeedClip;		
		BOOLEAN					m_bHasViewport;
		BOOLEAN					m_bFrequentChangeFlag;
		BOOLEAN					m_bCanMakeCache;

		UINT					m_uiAlpha;
		FLOAT					m_fRotateRadian;
		ERotateAxis				m_eRA;
		INT						m_iAnchorX;
		INT						m_iAnchorY;
		FLOAT					m_fScaleWidth;
		FLOAT					m_fScaleHeight;

		Array<CRenderBaseFilter>	m_aryFilter;
	};
}