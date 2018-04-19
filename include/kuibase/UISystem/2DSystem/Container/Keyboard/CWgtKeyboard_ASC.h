/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CWgtKeyboard_ASC : public CWgtKeyboard
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CWgtKeyboard_ASC(BOOLEAN bNumberBindLetter = FALSE);
		virtual ~CWgtKeyboard_ASC();

	protected:// method
		CWgtKeyboard_ASC(const CWgtKeyboard_ASC&){}
		CWgtKeyboard_ASC& operator =(const CWgtKeyboard_ASC&){return *this;}

	private:// method

	protected:// property
		virtual void buildKeyTable();

	private:// property
		BOOLEAN		  m_bNumberBindLetter;

		CKeyTable_ASC m_keyTableASC;
	};
}