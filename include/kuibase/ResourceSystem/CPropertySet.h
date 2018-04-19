/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CPropertySet : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CPropertySet();		
		virtual ~CPropertySet();


		virtual void			parse(const String &strData);
		virtual String			save();

		UINT					getPropertyCount();
		BOOLEAN					hasProperty(const String & strName);

		const String&			getPropertyString(const String& strName) const;
		INT						getPropertyInt(const String& strName) const;
		UINT					getPropertyUInt(const String& strName) const;
		FLOAT					getPropertyFloat(const String& strName) const;
		BOOLEAN					getPropertyBool(const String& strName) const;

		void					setProperty(const String& strName, const String& strValue);
		void					setProperty(const String& strName, INT iValue);
		void					setProperty(const String& strName, UINT uiValue);
		void					setProperty(const String& strName, FLOAT fValue);
		void					setProperty(const String& strName, BOOLEAN bValue);

		void					deleteProperty(const String& strName);
		void					clearAll();

		CPropertySet*			getPropertySet(const String& strName);

		virtual CPropertySet*	clone();
		virtual void			cloneTo(CPropertySet* pPropertySet);

	protected:// method
		CPropertySet(const CPropertySet &){}
		CPropertySet &operator = (const CPropertySet &){return *this;}

	private:// method
		void					updatePropertySet();

	protected:// property

	private:// property
		Map<String, String, tagStringCompare>			m_mapProperty;
		Map<String, CPropertySet, tagStringCompare>		m_mapPropertySet;

	};
}