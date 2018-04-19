/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../BaseProject/Framework/CBaseMessage.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgRender : public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum		

	public:// method		
		CMsgRender();
		virtual ~CMsgRender();

		virtual void	handleMessage();

	protected:// method
		CMsgRender(const CMsgRender&){}
		CMsgRender& operator =(const CMsgRender&){return *this;}
		
	private:// method

	protected:// property

	private:// property

	};
}