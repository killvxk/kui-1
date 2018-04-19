/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModelList : public CModel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CCtrlList;

	public:// method
		CModelList();
		virtual ~CModelList();

		/**
		* A flag indicates the first item of list is the first of your data or not.
		* If it is FALSE, when list control is going to show previous page items which don't exist in the model list, it will dispatch WANT_PREV_PAGE event.
		* The default value is TRUE.
		*/
		void			setReachToHead(BOOLEAN bValue)			{ m_bReachToHead = bValue; }
		BOOLEAN			getReachToHead() const					{ return m_bReachToHead; }

		/**
		* A flag indicates the last item of list is the last of your data or not.
		* If it is FALSE, when list control is going to show next page items which don't exist in the model list, it will dispatch WANT_NEXT_PAGE event.
		* The default value is TRUE.
		*/
		void			setReachToTail(BOOLEAN bValue)			{ m_bReachToTail = bValue; }
		BOOLEAN			getReachToTail() const					{ return m_bReachToTail; }

		/**
		* Get the count of items added into list 
		*/
		UINT			getItemCount()							{ return m_aryItem.size(); }		

		Object*			getItemAt(UINT uiIndex);

		INT				getItemIndex(Object *pObject);

		/**
		* @brief	Add an item into list.
		*
		* @param	pObject		The content of this item, it can't be null.
		*/
		void			addItem(Object* pObject);

		/**
		* @brief	Insert an item into list.
		*
		* @param	uiIndex		The index you want to insert to.
		* @param	pObject		The content of this item, it can't be null.
		*/
		void			insertItem(UINT uiIndex, Object* pObject);

		void			deleteItem(Object* pObject);		
		void			deleteItemAt(UINT uiIndex);
		void			deleteAllItems();

	protected:// method
		CModelList(const CModelList&){}
		CModelList& operator =(const CModelList&){return *this;}

	private:// method

	protected:// property

	private:// property
		Array2< ObjectPtr<Object> >	m_aryItem;

		BOOLEAN						m_bReachToHead;
		BOOLEAN						m_bReachToTail;
	};
}