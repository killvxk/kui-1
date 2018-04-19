/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResFilterSet : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResFilterSet();		
		virtual ~CResFilterSet();

		UINT				getFilterSize();
		CResFilter*			getFilterByIndex(UINT uiIndex);
		CResFilter*			createFilter(const String &strType);
		void				deleteFilter(CResFilter *pFilter);
		void				removeFilter(CResFilter *pFilter);
		void				addFilter(CResFilter *pFilter);
		void				clearAll();

		void				parse(CPropertySet* pPropertySet);
		void				save(CPropertySet* pPropertySet);

		void				cloneTo(CResFilterSet* pFilterSet);
	protected:// method
		CResFilterSet(const CResFilterSet &){}
		CResFilterSet &operator = (const CResFilterSet &){return *this;}

	private:// method

	protected:// property

	private:// property
		Array<CResFilter>	m_aryFilter;

	};
}