/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	ISoundGenerator	
	*			The interface for sound generator. You should implement this interface in your project via platform service supporting.
	*			And then, set the implement class to ENV.
	*/
	class OPENHMI_API ISoundGenerator
	{
	public:// method
		virtual			~ISoundGenerator() {}

		/**
		* @brief	When someone button is clicked, the system will callback this function.
		*
		* @param	iSoundType	The Sound type for different buttons.
		*/
		virtual	void	playButtonClickSound(INT iSoundType) = 0;
	};
}