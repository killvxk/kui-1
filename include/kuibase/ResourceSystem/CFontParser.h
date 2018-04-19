/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CFontParser
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EFontType
		{
			FT_TTF = 0,
			FT_BMP,
		};

		struct tagFontEntry
		{	
			EFontType	eFontType;
			String		strFontName;
			String		strFilePath;
			UINT		uiFaceIndex;
		};

	private:// embed class or struct or enum	

	public:// method
		static	CFontParser*	getInstance();
		static	void			deleteInstance();

		BOOLEAN					parse(const String &strPath);
		void					empty();

		UINT					getFontCount();
		const tagFontEntry*		getFontByIndex(UINT uiIndex);
		const tagFontEntry*		getFontByName(const String &strFontName);

	protected:// method
		CFontParser();
		virtual ~CFontParser();
 
	private:// method
		BOOLEAN		buildFontFamily(CNodeParser &parser);
		EFontType	strToFontType(const String *pstrType);

	protected:// property

	private:// property
		static	CFontParser	*s_pInstance;

		String m_strFontFolder;

		Map<String, tagFontEntry, tagStringCompare> m_mapFontEntry;		
	};
}