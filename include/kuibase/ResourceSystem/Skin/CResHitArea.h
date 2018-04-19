/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResHitArea : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResHitArea();
		virtual ~CResHitArea();

		CResHitArea*		clone();

	protected:// method
		CResHitArea(const CResHitArea &){}
		CResHitArea &operator = (const CResHitArea &){return *this;}	

	private:// method

	protected:// property

	private:// property

	};
}