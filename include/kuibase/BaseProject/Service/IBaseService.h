/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IBaseService
	{
	public:// method
		virtual			~IBaseService() {}

		/**
		* Set up connection to system communication layer.
		*/
		virtual	void	connect() = 0;

		/**
		* Break up connection to system communication layer.
		*/
		virtual	void	disconnect() = 0;

		/**
		* @brief	Indicates whether the service is available to provide service for logic or not.
		*
		* @return	TRUE	The service is available to provide service for logic.
		*			FALSE	The service is not available to provide service for logic.
		*/
		virtual BOOLEAN	isAvailable() = 0;
	};
}