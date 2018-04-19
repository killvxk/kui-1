/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseData : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseData();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void			setMethodID(UINT uiValue)			{ m_uiMethodID = uiValue; }
		UINT			getMethodID() const					{ return m_uiMethodID; }

		void			setErrorCode(INT iValue)			{ m_iErrorCode = iValue; }
		UINT			getErrorCode() const				{ return m_iErrorCode; }

		void			setErrorMsg(const String &strValue) { m_strErrorMsg = strValue; }
		const String &	getErrorMsg() const					{ return m_strErrorMsg; }

	protected:// method
		CBaseData();

	private:// method

	protected:// property

	private:// property
		UINT	m_uiMethodID;
		INT		m_iErrorCode;
		String	m_strErrorMsg;
	};
}