/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IServiceAdaptor
	{
	public:// method
		virtual			~IServiceAdaptor() {}

		/**
		* Initialize this service adapter.
		*/
		virtual BOOLEAN	initialize() = 0;

		/**
		* Uninitialize this service adapter.
		*/
		virtual void	unInitialize() = 0;
	};
}