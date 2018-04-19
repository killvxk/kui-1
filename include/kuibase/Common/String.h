/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	String	
	*			It is a common string class. The encoding of it is UTF-16.
	*/
	class OPENHMI_API String : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	INT		toInt(const CHAR *pcSrc);
		static	INT		toInt(const BYTE *pbtSrc);
		static	UINT	toUInt(const CHAR *pcSrc);
		static	UINT	toUInt(const BYTE *pbtSrc);
		static	FLOAT	toFloat(const CHAR *pcSrc);
		static	FLOAT	toFloat(const BYTE *pbtSrc);
		static	BOOLEAN	toBool(const CHAR *pcSrc);
		static	BOOLEAN	toBool(const BYTE *pbtSrc);

		static	String	toString(INT iValue);
		static	String	toString(UINT uiValue);
		static	String	toString(FLOAT fValue, UINT uiDotUintCount = 4);
		static	String	toString(BOOLEAN bValue);		

		static	void	setZero(CHAR *pcSrc, UINT uiLen);
		static	INT		strlen(const CHAR *pcSrc);
		static	INT		strstr(const CHAR *pcDest, const CHAR *pcSearch);
		static	INT		strstr_last(const CHAR *pcDest, const CHAR *pcSearch);
		static  BOOLEAN strstart(const CHAR* pcSrc, const CHAR *pcSub);
		static  BOOLEAN strend(const CHAR* pcSrc, const CHAR* pcSub);
		static	INT		strcmp(const CHAR *pcSrc1, const CHAR *pcSrc2);
		static	void	strcat(CHAR *pcDest, const CHAR *pcSrc);
		static	void	strcpy(CHAR *pcDest, const CHAR *pcSrc);
		static	void	strncpy(CHAR *pcDest, const CHAR *pcSrc, UINT uiLen);


		static	void	printf(const CHAR *pcFormat, ...);
		static	String	format(const CHAR *pcFormat, ...);

		static	void	convertFromBYTE_UTF16(const BYTE *pbtData, UINT uiLen, String &strOut);

		static	void	convertToMBCS(const CHAR *pcSrc, char *pcOut, UINT uiBufferLen);
		static	void	convertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen);
		static	void	convertFromMBCS(const char *pcSrc, String &strOut);

		static	void	convertToUTF8(const CHAR *pcSrc, BYTE *pbtOut, UINT uiBufferLen, UINT *puiOutLen = NULL);
		static	void	convertToUTF8(const String &strSrc, BYTE *pbtOut, UINT uiBufferLen, UINT *puiOutLen = NULL);
		static	void	convertFromUTF8(const BYTE *pbtSrc, String &strOut);

		static	void	encodeChars(const String &strSrc, UINT uiCharCount, const CHAR acCharTab[], const CHAR* apcEncStrTab[], String &strOut); 
		static	void	decodeChars(const String &strSrc, UINT uiCharCount, const CHAR acCharTab[], const CHAR* apcEncStrTab[], String &strOut);

		String();
		String(const CHAR *pcSrc);
		String(const CHAR *pcSrc, UINT uiLen);
		String(const String &strSrc);
		virtual ~String();

		UINT	length() const;
		CHAR	charAt(UINT uiIndex) const;
		INT		indexOf(const CHAR *pcSubString, UINT uiStartIndex = 0) const;
		INT		indexOf(const String &strSubString, UINT uiStartIndex = 0) const;
		INT		lastIndexOf(const CHAR *pcSubString) const;
		INT		lastIndexOf(const String &strSubString) const;
		INT		compare(const CHAR *pcSrc) const;
		INT		compare(const String &strSrc) const;

		String&	append(const CHAR *pcSrc);
		String	subString(UINT uiStartIndex, UINT uiLen = 0) const;
		void	replace(const CHAR cOld, const CHAR cNew);
		void	replaceFirst(const CHAR *pcOld, const CHAR *pcNew);	
		void	toUpperCase();
		void	toLowerCase();

		String& operator =(const CHAR *pcSrc);
		String& operator =(const String &strSrc);
		String	operator +(const CHAR *pcSrc) const;
		String	operator +(const String &strSrc) const;
		CHAR	operator [](UINT uiIndex) const;		
		BOOLEAN operator ==(const CHAR *pcSrc) const;
		BOOLEAN operator ==(const String &strSrc) const;
		BOOLEAN operator !=(const CHAR *pcSrc) const;
		BOOLEAN operator !=(const String &strSrc) const;

		operator const CHAR*() const
		{
			return m_pc;
		}

	protected:// method		

	private:// method
		static	BOOLEAN	strncmp(const CHAR *pcSrc1, const CHAR *pcSrc2, INT iLen);
		static	void	innerConvertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen);

		void	duplicate(const CHAR *pcSrc);
		void	free();

	protected:// property

	private:// property
		CHAR	*m_pc;
		UINT	m_uiLen;
	};

	extern const String OPENHMI_API STRING_NULL;
}