/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResScreen : public CResGroup
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResScreen();
		virtual ~CResScreen();

		BOOLEAN	isMainFrame();
		virtual BOOLEAN	merge(CResNode *pNode, BOOLEAN bFull);

	protected:// method
		CResScreen(const CResScreen &){}
		CResScreen &operator = (const CResScreen &){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}