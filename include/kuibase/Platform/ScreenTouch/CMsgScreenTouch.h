/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../BaseProject/Framework/CBaseMessage.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgScreenTouch : public CBaseMessage
	{
	public:// const define

	private:// const define		

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CMsgScreenTouch(IScreenTouchListener *pListener, const CCursor &cursor);
		virtual ~CMsgScreenTouch();

		virtual void	handleMessage();

	protected:// method
		CMsgScreenTouch(const CMsgScreenTouch&){}
		CMsgScreenTouch& operator =(const CMsgScreenTouch&){return *this;}

	private:// method

	protected:// property

	public:// property
		IScreenTouchListener				*m_pListener;
		CCursor								m_cursor;
	};
}