/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API ISocketListener
	{
	public:// method
		virtual			~ISocketListener() {}

		virtual void	onReceiveData(INT iBufferSize, BYTE * pbtBuffer, CAutoSocket *pSocket) = 0;	
	};
}