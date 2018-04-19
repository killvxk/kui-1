/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	CNodeParser	
	*			This is a parser for ini format file. The ini format file has a session list, and there are key-value pairs in each session.
	*			You can use this parser for your special startup configuration file. In that file, maybe save some setting information, such as the user selected language.
	*/
	class OPENHMI_API CNodeParser : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagSection
		{
			Map<String, String, tagStringCompare>	mapItem;
		};

		enum EParserState
		{
			PS_SECTION_START	= 1,
			PS_SECTION_END		= 2,
			PS_KEY_START		= 3,
			PS_KEY_END			= 4,
			PS_VALUE			= 5,
			PS_NEXT				= 6
		};

	public:// method
		CNodeParser();
		virtual ~CNodeParser();

		BOOLEAN									parse(const String &strPath);
		BOOLEAN									save(const String &strPath);

		UINT									getSectionCount(){ return m_mapSection.size(); };
		Map<String, String, tagStringCompare>*	getSection(const String &strSection);
		
		const String&							getItemString(const String &strSectionName, const String &strKey);
		INT										getItemInt(const String &strSectionName, const String &strKey);
		UINT									getItemUInt(const String &strSectionName, const String &strKey);
		FLOAT									getItemFloat(const String &strSectionName, const String &strKey);
		BOOLEAN									getItemBool(const String &strSectionName, const String &strKey);

		void									createSection(const String &strSection);
		void									deleteSection(const String &strSection);

		void									setItem(const String &strSectionName, const String &strKey, const String &strValue);
		void									setItem(const String &strSectionName, const String &strKey, INT iValue);
		void									setItem(const String &strSectionName, const String &strKey, UINT uiValue);
		void									setItem(const String &strSectionName, const String &strKey, FLOAT fValue);
		void									setItem(const String &strSectionName, const String &strKey, BOOLEAN bValue);		

		static void								parseProperty(const String &strData, CPropertySet &propertySet);
		static String							saveProperty(CPropertySet &propertySet);
		
		static void								parseStrValue(const String &strData, Array<String> &aryValue);
		static String							saveStrValue(const Array<String> &aryValue);
		static void								parseUIntValue(const String &strData, Array2<UINT> &aryValue);
		static String							saveUIntValue(Array2<UINT> &aryValue);							

	protected:// method
		CNodeParser(const CNodeParser&){}
		CNodeParser& operator =(const CNodeParser&){return *this;}

	private:// method
		void			parseFile(const CHAR *pcData);
		const String&	getItemValue(const String &strSectionName, const String &strKey);

	protected:// property

	private:// property
		Map<String, tagSection, tagStringCompare>	m_mapSection;
	};
}