/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IViewTree
	{
	public:// method
		/**
		* @brief	Get a component according to the tree content.
		*
		* @param	pTree		The pointer of the tree.
		* @param	pObjItem	The content of a tree item, generally we use this to specify tree item's component.
		* @return	The component pointer specified by the content.
		*/
		virtual CComponent* getItemComponent(CCtrlTree* pTree, Object* pObjItem) = 0;
		
		/**
		* @brief	Draw the item according to its status.
		*			Usually you needn't overwrite this interface if you want to draw it in default way.
		*
		* @param	pTree		The pointer of the tree.
		* @param	pObjItem	The content for this item.
		* @param	pComponent	The component pointer of this item.
		* @param	bSelected	The select status of this item.
		* @param	bFocus		The touch focus status of this item.
		* @param	bDisable	The disable status of this item.
		*/
		virtual void drawItem(	CCtrlTree* pTree, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable) = 0;
	};
}