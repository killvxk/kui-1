/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CLanguageParser
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	CLanguageParser*	getInstance();
		static	void				deleteInstance();

		BOOLEAN			parse(const String &strPath);
		BOOLEAN			save(const String &strPath);

		const String&	getString(UINT uiID);
		void			setString(UINT uiID, const String& strData);

	protected:// method
		CLanguageParser();
		virtual ~CLanguageParser();

	private:// method
		void			buildStringTable(Map<String, String, tagStringCompare>* pSection);
	
	protected:// property

	private:// property
		static	CLanguageParser	*s_pInstance;

		Map<UINT, String, tagNumCompare<UINT> >	m_mapStringTable;
	};
}