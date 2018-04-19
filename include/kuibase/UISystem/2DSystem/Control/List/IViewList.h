/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IViewList
	{
	public:// method
		/**
		* @brief	Get a component according to the list content.
		*
		* @param	pList		The pointer of the list.
		* @param	pObjItem	The content of a list item, generally we use this to specify list item's component.
		* @return	The component pointer specified by the content.
		*/
		virtual CComponent* getItemComponent(CCtrlList* pList, Object* pObjItem) = 0;
		
		/**
		* @brief	Draw the item according to its status.
		*			Usually you needn't overwrite this interface if you want to draw it in default way.
		*
		* @param	pList		The pointer of the list.
		* @param	pObjItem	The content for this item.
		* @param	pComponent	The component pointer of this item.
		* @param	bSelected	The select status of this item.
		* @param	bFocus		The touch focus status of this item.
		* @param	bDisable	The disable status of this item.
		*/
		virtual void drawItem(	CCtrlList* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable) = 0;
			
		/**
		* @brief	Draw the bubble.
		*
		* @param	pList		The pointer of the list.
		* @param	pObjItem	The content of this item, generally we use this to specify list item's component.
		* @param	pBubble		The pointer of the bubble component.
		*/
		virtual void drawBubble(CCtrlList* pList, Object* pObjItem, CComponent* pBubble) = 0;
	};
}