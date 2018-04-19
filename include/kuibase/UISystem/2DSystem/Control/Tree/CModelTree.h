/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModelTree : public CModel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		friend class CCtrlTree;

		struct tagItemNode
		{
			ObjectPtr<Object>	ptrObject;
			UINT				uiLevel;

			tagItemNode(Object* pObj, UINT	uiLev = 0) 
				: ptrObject(pObj)
				, uiLevel(uiLev)
			{
				;
			}
		};

	public:// method
		CModelTree();
		virtual ~CModelTree();

		UINT			getItemCount()	{ return m_aryItem.size(); }		
		Object*			getItemAt(UINT uiIndex);

		/**
		* Get the item level of the item specified by index.
		*/
		UINT			getItemLevel(UINT uiIndex);

		INT				getItemIndex(Object *pObject);

		/**
		* @brief	Add an item into the tree.
		*
		* @param	pObject		The content of this item, it can't be null.
		* @param	uiLevel		Used to define the tree level. 
		*						The level of root item must is 0, the level of sub item is bigger than the level of parent item by 1.
		*						The brother items have the same level.
		*/
		void			addItem(Object* pObject, UINT uiLevel = 0);

		/**
		* @brief	Insert an item into the tree.
		*
		* @param	uiIndex		The index you want to insert to.
		* @param	pObject		The content of this item, it can't be null.
		* @param	uiLevel		Used to define the tree level.
		*/
		void			insertItem(UINT uiIndex, Object* pObject, UINT uiLevel = 0);

		void			deleteItem(Object* pObject);		
		void			deleteItemAt(UINT uiIndex);
		void			deleteAllItems();

	protected:// method
		CModelTree(const CModelTree&){}
		CModelTree& operator =(const CModelTree&){return *this;}

	private:// method
		tagItemNode*	getItemNodeAt(UINT uiIndex) { return m_aryItem[uiIndex]; }

	protected:// property

	private:// property
		Array<tagItemNode>		m_aryItem;
	};
}