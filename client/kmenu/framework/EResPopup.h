#pragma once

#include "OpenHMI.h"

namespace OpenHMI
{
	enum EResPopup
	{
		RES_POP_UNKNOWN			= 0,

		// TODO: define popup ID here
		RES_POP_TIPS										 = 2 ,
		RES_POP_CHANGE_SKIN									 = 3 ,
		RES_POP_ADMIN										 = 4 ,
		RES_POP_UDISK_REG									 = 5 ,
		RES_POP_UDISK_LOGIN									 = 6 ,
		RES_POP_UDISK										 = 8 ,
		RES_POP_CHANGE_PSW									 = 9 ,
		RES_POP_UPLOADCFG									 = 10 ,
		RES_POP_SAVE_FILE									 = 11 ,
		RES_POP_READ_FILE									 = 12 ,
	};
}