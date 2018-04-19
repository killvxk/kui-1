#pragma once

#include "OpenHMI.h"

namespace OpenHMI
{
	enum EResFuncWidget
	{
		RES_FUNCWGT_UNKNOWN			= 0,

		// TODO: define function widget ID here
		RES_FUNCWGT_CHANGE_SKIN									 = 2 ,
		RES_FUNCWGT_OP_MENU										 = 4 ,
		RES_FUNCWGT_AD											 = 5 ,
		RES_FUNCWGT_USER										 = 6 ,
		RES_FUNCWGT_POP_USER									 = 8 ,
		RES_FUNCWGT_SKIN_ITEM									 = 10 ,
		RES_FUNCWGT_ADMIN										 = 12 ,
		RES_FUNCWGT_UDISK_LOGIN									 = 13 ,
		RES_FUNCWGT_UDISK_REG									 = 15 ,
		RES_FUNCWGT_TIPS										 = 16 ,
		RES_FUNCWGT_HOT_SEARCH									 = 18 ,
		RES_FUNCWGT_UDISK										 = 19 ,
		RES_FUNCWGT_CHANGE_PSW									 = 20 ,
		RES_FUNCWGT_UPLOADCFG									 = 21 ,
		RES_FUNCWGT_SAVE_FILE									 = 22 ,
		RES_FUNCWGT_READ_FILE									 = 23 ,
		RES_FUNCWGT_NO_RESULT									 = 27 ,
		RES_FUNCWGT_AD_REC										 = 28 ,
	};
}