/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CKeyTable : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~CKeyTable();

		/**
		*  Inner method.
		*/
		void setKeyType(UINT uiType);

		UINT getKeyType() const { return m_uiKeyType; }
		
		/**
		*  Inner method.
		*/
		void setKeyCode(UINT uiCode);

		UINT getKeyCode() const { return m_uiKeyCode; }

	protected:// method
		CKeyTable();

	private:// method

	protected:// property
		UINT		m_uiKeyType;
		UINT		m_uiKeyCode;

	private:// property

	};
}