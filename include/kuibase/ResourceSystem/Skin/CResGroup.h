/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResGroup : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagCompareByZOrder
		{
			INT operator()(const CResItem *pItem1, const CResItem *pItem2) const;
		};

	public:// method		
		virtual ~CResGroup();

		BOOLEAN			isScreen()			{ return getType().compare(L"SCREEN") == 0; }
		BOOLEAN			isPopup()			{ return getType().compare(L"POPUP") == 0; }
		BOOLEAN			isFuncWidget()		{ return getType().compare(L"FUNCWIDGET") == 0; }
		BOOLEAN			isCtrlWidget()		{ return getType().compare(L"CTRLWIDGET") == 0; }

		UINT			getItemSize();
		CResItem*		getItemByIndex(UINT uiIndex);
		CResItem*		getItemByID(UINT uiID);
		CResItem*		getItemByName(const String &strName);
		CResItem*		createItem(const String &strType);
		CResItem*		createFuncWidget(UINT uiRefID);
		CResItem*		createCtrlWidget(UINT uiRefID);
		void			deleteItem(CResItem *pItem);
		void			removeItem(CResItem *pItem);
		void			addItem(CResItem *pItem, BOOLEAN bNeedReset);

		UINT			getFilterSize();
		CResFilter*		getFilterByIndex(UINT uiIndex);
		CResFilter*		createFilter(const String &strType);
		void			deleteFilter(CResFilter *pFilter);
		void			removeFilter(CResFilter *pFilter);
		void			addFilter(CResFilter *pFilter);
		void			deleteAllFilter();

		void			deleteAllTextField();
		void			deleteAllImageField();
		void			notifyItemZOrderChanged();

		void			parse(const Map<String, String, tagStringCompare> *pmapData);
		void			save(Map<String, String, tagStringCompare> *pmapData);
		virtual BOOLEAN	merge(CResNode *pNode, BOOLEAN bFull);
		virtual void	clearMergeFlag();

		virtual void	parseProperty(const String &strData);
		virtual String	saveProperty();

	protected:// method
		CResGroup();

	private:// method
		void			getFilterList();
		void			updateFilterProperty();

		void			buildItemArray();

		BOOLEAN			mergeChildren(CResGroup* pMergeSrc);

	protected:// property

	private:// property
		Map<UINT, CResItem, tagNumCompare<UINT> >	m_mapItem;
		Array2<CResItem*>							m_aryItem;
		CResFilterSet								m_filterSet;

		UINT	m_uiMaxItemID;
	};
}