/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	tagNumCompare	
	*			It is used in Map, Map2 or Set for key comparing.
	*/
	template<typename TNum>
	struct tagNumCompare
	{
		INT operator()(TNum param1, TNum param2) const
		{
			if( param1 == param2 )
			{
				return 0;
			}
			else if( param1 > param2 )
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	};

	/**
	* @class	tagStringCompare	
	*			It is used in Map, Map2 or Set for key comparing.
	*/
	struct OPENHMI_API tagStringCompare
	{
		INT operator()(const String &str1, const String &str2) const
		{
			return str1.compare(str2);
		}
	};
}