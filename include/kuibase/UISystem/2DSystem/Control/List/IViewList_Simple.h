/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IViewList_Simple
	{
	public:// method
		/**
		* @brief	Get a component for each list item.
		*
		* @return	The component pointer.
		*/
		virtual CComponent* getItemComponent(CCtrlList_Simple* pList) = 0;
		
		/**
		* @brief	Draw the item according to its status.
		*
		* @param	pObjItem	The content for this item.
		* @param	pComponent	The component pointer of this item.
		* @param	bSelected	The select status of this item.
		* @param	bFocus		The touch focus status of this item.
		*/
		virtual void drawItem(	CCtrlList_Simple* pList,
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus) = 0;
	};
}