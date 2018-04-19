/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CArc : public CBasePath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CArc();
		virtual ~CArc();
		
		virtual	void smashToSegments(INT iSegmentCount);

	protected:// method
		CArc(const CArc&){}
		CArc& operator =(const CArc&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}