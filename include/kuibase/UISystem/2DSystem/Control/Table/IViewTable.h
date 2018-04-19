/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IViewTable
	{
	public:// method
		/**
		* @brief	Get a component according to the table content.
		*
		* @param	pTable		The pointer of the table.
		* @param	pObjItem	The pointer of a table item, generally we use this to specify table item's component.
		* @return	The component pointer specified by the content.
		*/
		virtual CComponent* getItemComponent(CCtrlTable* pTable, Object* pObjItem) = 0;
		
		/**
		* @brief	Draw the item according to its status.
		*			Usually you needn't overwrite this interface if you want to draw it in default way.
		*
		* @param	pTable		The pointer of the table.
		* @param	pObjItem	The content for this item.
		* @param	pComponent	The component pointer of this item.
		* @param	bSelected	The select status of this item.
		* @param	bFocus		The focus status of this item.
		* @param	bDisable	The disable status of this item.
		*/
		virtual void drawItem(	CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable) = 0;
	};
}