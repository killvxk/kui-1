/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderItem : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ERenderItemType
		{
			RIT_UNKNOWN = 0,
			RIT_GROUP,
			RIT_TEXT ,
			RIT_IMAGE,
			RIT_VIEWPORT,
		};

	private:// embed class or struct or enum

	public:// method
		virtual ~CRenderItem();

		ERenderItemType		getType() const								{ return m_eType;}

		void				setParent(CRenderGroup *pValue)				{ m_pParent = pValue; }
		CRenderGroup*		getParent()	const							{ return m_pParent; }

		void				setCacheData(CRenderCacheData *pCacheData)	{ m_ptrCacheData = pCacheData; }
		CRenderCacheData*	getCacheData()								{ return m_ptrCacheData; }
		void				setUserData(Object	*pData)					{ m_ptrUserData = pData;}
		Object*				getUserData()								{ return m_ptrUserData; }

		void				setX(INT iValue);
		INT					getX() const								{ return m_iX; }
		void				setY(INT iValue);
		INT					getY() const								{ return m_iY; }
		void				setWidth(UINT uiValue);
		UINT				getWidth() const							{ return m_uiWidth; }
		void				setHeight(UINT uiValue);
		UINT				getHeight() const							{ return m_uiHeight; }
		
		virtual CRenderItem*	hitTest(INT iX, INT iY);

	protected:// method
		CRenderItem();	

		virtual void	notifySizePropertyChanged();
		virtual void	notifyMergePropertyChanged();
		virtual void	notifyContentPropertyChanged();
		virtual	void	notifyChildrenChanged();

	private:// method

	protected:// property	
		ERenderItemType	m_eType;

	private:// property		
		CRenderGroup					*m_pParent;

		ObjectPtr<CRenderCacheData>		m_ptrCacheData;
		ObjectPtr<Object>				m_ptrUserData;

		INT				m_iX;
		INT				m_iY;
		UINT			m_uiWidth;
		UINT			m_uiHeight;		
	};
}