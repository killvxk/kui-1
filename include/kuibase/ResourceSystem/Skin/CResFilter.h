/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResFilter : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResFilter();
		virtual ~CResFilter();

		CResFilter*		clone();

	protected:// method
		CResFilter(const CResFilter &){}
		CResFilter &operator = (const CResFilter &){return *this;}	

	private:// method

	protected:// property

	private:// property

	};
}