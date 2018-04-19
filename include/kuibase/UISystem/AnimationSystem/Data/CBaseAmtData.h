/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseAmtData : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseAmtData();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData)		{ return NULL; };
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData)	{ return NULL; };
		virtual CBaseAmtData*	multiply(FLOAT fValue)			{ return NULL; };
		virtual CBaseAmtData*	divide(FLOAT fValue)			{ return NULL; };	
		virtual CBaseAmtData*	cloneData()						{ return NULL; };
		virtual void			set(CBaseAmtData* pData)		{ };

	protected:// method
		CBaseAmtData();

	private:// method

	protected:// property

	private:// property

	};
}