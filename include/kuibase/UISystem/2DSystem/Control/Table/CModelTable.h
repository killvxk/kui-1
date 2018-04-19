/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModelTable : public CModel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CCtrlTable;

	public:// method
		CModelTable();
		virtual ~CModelTable();

		/**
		* Real item count means the items added or to be added. This count is always >= the table's item count.
		*/
		void			setRealItemCount(UINT uiValue)			{ m_uiRealItemCount = uiValue; }
		UINT			getRealItemCount()						{ return m_uiRealItemCount; }

		/**
		* Get the count of items added into table 
		*/
		UINT			getItemCount()							{ return m_aryItem.size(); }		


		Object*			getItemAt(UINT uiIndex);

		INT				getItemIndex(Object *pObject);

		/**
		* @brief	Add an item into table.
		*
		* @param	pObject		The content of this item, if it is null, which means a empty table cell.
		*/
		void			addItem(Object* pObject);

		/**
		* @brief	Insert an item into table.
		*
		* @param	uiIndex		The index you want to insert to.
		* @param	pObject		The content of this item, if it is null, which means a empty table cell.
		*/
		void			insertItem(UINT uiIndex, Object* pObject);

		void			deleteItem(Object* pObject);
		void			deleteItemAt(UINT uiIndex);
		void			deleteAllItems();

	protected:// method
		CModelTable(const CModelTable&){}
		CModelTable& operator =(const CModelTable&){return *this;}

	private:// method

	protected:// property

	private:// property
		Array2< ObjectPtr<Object> >	m_aryItem;

		UINT						m_uiRealItemCount;
	};
}