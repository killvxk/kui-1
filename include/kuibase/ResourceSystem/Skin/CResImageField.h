/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResImageField : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResImageField();
		virtual ~CResImageField();

		CResImageField*		clone();

		virtual void	parseProperty(const String &strData);
		virtual String	saveProperty();

	protected:// method
		CResImageField(const CResImageField &){}
		CResImageField &operator = (const CResImageField &){return *this;}	

	private:// method

	protected:// property

	private:// property

	};
}