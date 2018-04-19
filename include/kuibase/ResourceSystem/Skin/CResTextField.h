/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResTextField : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResTextField();
		virtual ~CResTextField();

		CResTextField*		clone();

		virtual void		parseProperty(const String &strData);
		virtual String		saveProperty();

	protected:// method
		CResTextField(const CResTextField &){}
		CResTextField &operator = (const CResTextField &){return *this;}	

	private:// method

	protected:// property

	private:// property

	};
}