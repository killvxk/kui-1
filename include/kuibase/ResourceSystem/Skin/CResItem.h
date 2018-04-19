/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResItem : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResItem();		
		virtual ~CResItem();

		void				setParent(CResGroup *pGroup);
		CResGroup*			getParent()						{ return m_pParent; }
		CResGroup*			getRawParent()					{ return m_pRawParent; }

		void				setVisible(BOOLEAN bValue);
		BOOLEAN				getVisible() const				{ return m_bVisible; }		
		void				setZOrder(UINT uiValue);
		UINT				getZOrder() const				{ return m_uiZOrder; }
		void				setRefID(UINT uiValue);
		UINT				getRefID() const				{ return m_uiRefID; }

		UINT				getTextFieldSize();
		CResTextField*		getTextFieldByIndex(UINT uiIndex);
		CResTextField*		createTextField();
		void				deleteTextField(CResTextField *pTextField);
		void				removeTextField(CResTextField *pTextField);
		void				addTextField(CResTextField *pTextField);
		void				deleteAllTextField();

		UINT				getImageFieldSize();
		CResImageField*		getImageFieldByIndex(UINT uiIndex);
		CResImageField*		createImageField();
		void				deleteImageField(CResImageField *pImageField);
		void				removeImageField(CResImageField *pImageField);
		void				addImageField(CResImageField *pImageField);
		void				deleteAllImageField();

		UINT				getHitAreaSize();
		CResHitArea*		getHitAreaByIndex(UINT uiIndex);
		CResHitArea*		createHitArea(const String &strType);
		void				deleteHitArea(CResHitArea *pHitArea);
		void				removeHitArea(CResHitArea *pHitArea);
		void				addHitArea(CResHitArea *pHitArea);
		void				deleteAllHitArea();

		UINT				getFilterSize();
		CResFilter*			getFilterByIndex(UINT uiIndex);
		CResFilter*			createFilter(const String &strType);
		void				deleteFilter(CResFilter *pFilter);
		void				removeFilter(CResFilter *pFilter);
		void				addFilter(CResFilter *pFilter);
		void				deleteAllFilter();

		CResItem*			clone();
		virtual BOOLEAN		merge(CResNode *pNode, BOOLEAN bFull);
		virtual void		clearMergeFlag();

		virtual void		parseProperty(const String &strData);
		virtual String		saveProperty();

	protected:// method
		CResItem(CResItem &itemObj);
		CResItem &operator = (const CResItem &){return *this;}

		virtual	void	notifyPropertyChanged();
		virtual	void	cloneTo(CResNode *pNode);

	private:// method
		void	getTextFieldList();
		void	updateTextFieldProperty();
		void	getImageFieldList();
		void	updateImageFieldProperty();
		void	getHitAreaList();
		void	updateHitAreaProperty();
		void	getFilterList();
		void	updateFilterProperty();

		BOOLEAN compareTextField(CResItem* pResItem);
		BOOLEAN compareImageField(CResItem* pResItem);

	protected:// property

	private:// property
		CResGroup*			m_pParent;
		CResGroup*			m_pRawParent;
		BOOLEAN				m_bVisible;
		UINT				m_uiZOrder;
		UINT				m_uiRefID;

		Array2<UINT>		m_aryTextField;
		Array2<UINT>		m_aryImageField;
		Array<CResHitArea>	m_aryHitArea;
		CResFilterSet		m_filterSet;
	};
}