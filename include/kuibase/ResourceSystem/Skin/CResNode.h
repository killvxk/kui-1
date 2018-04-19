/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResNode : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

		enum EMergeState
		{
			MS_UNKNOWN		= 0,
			MS_NORMAL		= 1,
			MS_MERGED		= 2,
		};

	public:// method		
		virtual ~CResNode();

		const String&		getPropertyString(const String& strName) const;
		INT					getPropertyInt(const String& strName) const;
		UINT				getPropertyUInt(const String& strName) const;
		FLOAT				getPropertyFloat(const String& strName) const;
		BOOLEAN				getPropertyBool(const String& strName) const;

		void				setProperty(const String& strName, const String& strValue);
		void				setProperty(const String& strName, INT iValue);
		void				setProperty(const String& strName, UINT uiValue);
		void				setProperty(const String& strName, FLOAT fValue);
		void				setProperty(const String& strName, BOOLEAN bValue);

		void				deleteProperty(const String& strName);

		CPropertySet*		getPropertySet(const String& strName);

		void				setID(UINT uiValue);
		UINT				getID() const					{ return m_uiID;	}

		void				setName(const String& strValue);
		const String&		getName() const					{ return m_strName; }

		void				setType(const String &strValue);
		const String&		getType() const					{ return m_strType; }

		virtual BOOLEAN		merge(CResNode *pNode, BOOLEAN bFull);

		virtual void		parseProperty(const String &strData);
		virtual String		saveProperty();

		void				setMergeFlag();
		BOOLEAN				hasMergeFlag();
		virtual void		clearMergeFlag();

	protected:// method
		CResNode();
				
		virtual	void		notifyPropertyChanged();
		virtual	void		cloneTo(CResNode *pNode);

	private:// method

	protected:// property
		CPropertySet		m_propertySet;

	private:// property
		UINT				m_uiID;
		String				m_strName;
		String				m_strType;

		EMergeState			m_eMergeState;
	};
}