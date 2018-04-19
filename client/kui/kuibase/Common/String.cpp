#include "OpenHMI.h"

#ifdef OS_WIN
	#include <stdarg.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <locale.h>
#endif

#ifdef OS_QNX
	#include <stdarg.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <wchar.h>
	#include <memory.h>
#endif

#ifdef OS_LINUX
	#include <stdarg.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <wchar.h>
	#include <memory.h>
#endif

namespace OpenHMI
{
	const String STRING_NULL;

	INT String::toInt(const CHAR *pcSrc)
	{
		if (pcSrc == NULL)
		{
			return 0;
		}

		BOOLEAN bNegative = FALSE;
		if (*pcSrc == L'-')
		{
			bNegative = TRUE;
			pcSrc++;
		}

		INT iValue = 0;
		while (L'0' <= *pcSrc && *pcSrc <= L'9')
		{
			iValue = iValue * 10 + (*pcSrc - L'0');
			pcSrc++;
		}

		return bNegative ? -iValue : iValue;
	}

	INT String::toInt(const BYTE *pbtSrc)
	{
		if (pbtSrc == NULL)
		{
			return 0;
		}

		BOOLEAN bNegative = FALSE;
		if (*pbtSrc == '-')
		{
			bNegative = TRUE;
			pbtSrc++;
		}

		INT iValue = 0;

		while ('0' <= *pbtSrc && *pbtSrc <= '9')
		{
			iValue = iValue * 10 + (*pbtSrc - '0');
			pbtSrc++;
		}

		return bNegative ? -iValue : iValue;
	}

	UINT String::toUInt(const CHAR *pcSrc)
	{
		if (pcSrc == NULL)
		{
			return 0;
		}

		UINT uiValue = 0;

		if (*pcSrc == L'0' && (pcSrc+1 != NULL) && *(pcSrc + 1) == L'x')
		{
			pcSrc = pcSrc + 2;

			while (pcSrc != NULL && *pcSrc != L'\0')
			{
				if (L'0' <= *pcSrc && *pcSrc <= L'9')
				{
					uiValue = uiValue * 16 + (*pcSrc - L'0');
				}
				else if (L'A' <= *pcSrc && *pcSrc <= L'F')
				{
					uiValue = uiValue * 16 + (*pcSrc - L'A') + 10;
				}
				else if (L'a' <= *pcSrc && *pcSrc <= L'f')
				{
					uiValue = uiValue * 16 + (*pcSrc - L'a') + 10;
				}
				pcSrc++;
			}
		}
		else
		{
			while (L'0' <= *pcSrc && *pcSrc <= L'9')
			{
				uiValue = uiValue * 10 + (*pcSrc - L'0');
				pcSrc++;
			}
		}

		return uiValue;
	}

	UINT String::toUInt(const BYTE *pbtSrc)
	{
		if (pbtSrc == NULL)
		{
			return 0;
		}

		UINT uiValue = 0;

		if (*pbtSrc == '0' && (pbtSrc+1 != NULL) && *(pbtSrc + 1) == 'x')
		{
			pbtSrc = pbtSrc + 2;

			while (pbtSrc != NULL && *pbtSrc != '\0')
			{
				if ('0' <= *pbtSrc && *pbtSrc <= '9')
				{
					uiValue = uiValue * 16 + (*pbtSrc - '0');
				}
				else if ('A' <= *pbtSrc && *pbtSrc <= 'F')
				{
					uiValue = uiValue * 16 + (*pbtSrc - 'A') + 10;
				}
				else if ('a' <= *pbtSrc && *pbtSrc <= 'f')
				{
					uiValue = uiValue * 16 + (*pbtSrc - 'a') + 10;
				}
				pbtSrc++;
			}
		}
		else
		{
			while ('0' <= *pbtSrc && *pbtSrc <= '9')
			{
				uiValue = uiValue * 10 + (*pbtSrc - '0');
				pbtSrc++;
			}
		}

		return uiValue;
	}

	FLOAT String::toFloat(const CHAR *pcSrc)
	{
		if (pcSrc == NULL)
		{
			return 0;
		}

		BOOLEAN bNegative = FALSE;
		if (*pcSrc == L'-')
		{
			bNegative = TRUE;
			pcSrc++;
		}

		FLOAT fValue = 0;
		while (L'0' <= *pcSrc && *pcSrc <= L'9')
		{
			fValue = fValue * 10 + (*pcSrc - L'0');
			pcSrc++;
		}

		if (*pcSrc == L'.')
		{
			pcSrc++;
		}

		FLOAT fTag = 10.0f;
		while (L'0' <= *pcSrc && *pcSrc <= L'9')
		{
			fValue += (*pcSrc - L'0') / fTag;
			fTag *= 10.0f;
			pcSrc++;
		}

		return bNegative ? -fValue : fValue;
	}

	FLOAT String::toFloat(const BYTE *pbtSrc)
	{
		if (pbtSrc == NULL)
		{
			return 0;
		}

		BOOLEAN bNegative = FALSE;
		if (*pbtSrc == '-')
		{
			bNegative = TRUE;
			pbtSrc++;
		}

		FLOAT fValue = 0;
		while ('0' <= *pbtSrc && *pbtSrc <= '9')
		{
			fValue = fValue * 10 + (*pbtSrc - '0');
			pbtSrc++;
		}

		if (*pbtSrc == '.')
		{
			pbtSrc++;
		}

		FLOAT fTag = 10.0f;
		while ('0' <= *pbtSrc && *pbtSrc <= '9')
		{
			fValue += (*pbtSrc -L'0') / fTag;
			fTag *= 10.0f;
			pbtSrc++;
		}

		return bNegative ? -fValue : fValue;
	}

	BOOLEAN String::toBool(const CHAR *pcSrc)
	{
		if (   pcSrc == NULL
			|| *(pcSrc) != L't'
			|| *(pcSrc + 1) != L'r'
			|| *(pcSrc + 2) != L'u'
			|| *(pcSrc + 3) != L'e'
			|| *(pcSrc + 4) != L'\0' )
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOLEAN String::toBool(const BYTE *pbtSrc)
	{
		if (   pbtSrc == NULL
			|| *(pbtSrc) != 't'
			|| *(pbtSrc + 1) != 'r'
			|| *(pbtSrc + 2) != 'u'
			|| *(pbtSrc + 3) != 'e'
			|| *(pbtSrc + 4) != '\0' )
		{
			return FALSE;
		}

		return TRUE;
	}

	String String::toString(INT iValue)
	{		
		CHAR acTemp[32] = {0};

		INT iStart = 0;
		if (iValue < 0)
		{
			acTemp[0] = L'-';
			iStart = 1;

			iValue = -iValue;
		}
		else if (iValue == 0)
		{
			acTemp[0] = L'0';
			return acTemp;
		}

		INT i = iStart;
		while (iValue > 0)
		{
			acTemp[i++] = (iValue % 10 + L'0');
			iValue /= 10;
		}

		INT iEnd = i - 1;
		while (iStart < iEnd)
		{
			CHAR cTemp = acTemp[iStart];
			acTemp[iStart] = acTemp[iEnd];
			acTemp[iEnd] = cTemp;

			iStart++;
			iEnd--;
		}

		return acTemp;
	}

	String String::toString(UINT uiValue)
	{
		CHAR acTemp[32] = {0};

		INT iStart = 0;
		if (uiValue == 0)
		{
			acTemp[0] = L'0';
			return acTemp;
		}

		INT i = iStart;
		while (uiValue > 0)
		{
			acTemp[i++] = (uiValue % 10 + L'0');
			uiValue /= 10;
		}

		INT iEnd = i - 1;
		while (iStart < iEnd)
		{
			CHAR cTemp = acTemp[iStart];
			acTemp[iStart] = acTemp[iEnd];
			acTemp[iEnd] = cTemp;

			iStart++;
			iEnd--;
		}

		return acTemp;
	}

	String String::toString(FLOAT fValue, UINT uiDotUintCount)
	{
		CHAR acFormat[10] = {0};
		CHAR acValue[32] = {0};

#ifdef OS_WIN		
		swprintf(acFormat, 10, L"%%.%df", uiDotUintCount);
		swprintf(acValue, 32, acFormat, fValue);
#endif

#ifdef OS_QNX
		swprintf(acFormat, 10, L"%%.%df", uiDotUintCount);
		swprintf(acValue, 32, acFormat, fValue);
#endif

#ifdef OS_LINUX
		swprintf(acFormat, 10, L"%%.%df", uiDotUintCount);
		swprintf(acValue, 32, acFormat, fValue);
#endif

		return acValue;
	}

	String String::toString(BOOLEAN bValue)
	{
		return bValue ? L"true" : L"false";
	}

	void String::setZero(CHAR *pcSrc, UINT uiLen)
	{
		if (pcSrc == NULL)
		{
			return;
		}

		CHAR *pcTail = pcSrc + uiLen;
		while (pcSrc < pcTail)
		{
			*pcSrc++ = NULL;
		}
	}

	INT	String::strlen(const CHAR *pcSrc)
	{
		if (pcSrc == NULL)
		{
			return 0;
		}

		INT iLen = 0;
		while (*pcSrc++ != NULL)
		{
			iLen++;
		}

		return iLen;
	}

	INT String::strstr(const CHAR *pcDest, const CHAR *pcSearch)
	{
		if (pcDest == NULL || *pcDest == NULL || pcSearch == NULL || *pcSearch == NULL)
		{
			return -1;
		}

		INT iDestLen = strlen(pcDest);
		INT iSearchLen = strlen(pcSearch);

		if (iDestLen < iSearchLen)
		{
			return -1;
		}

		const CHAR *pcCur = pcDest;
		const CHAR *pcTail = pcDest + (iDestLen - iSearchLen);

		while (pcCur <= pcTail)
		{
			if (strncmp(pcCur, pcSearch, iSearchLen))
			{
				return (pcCur - pcDest);
			}

			pcCur++;
		}

		return -1;
	}

	INT String::strstr_last(const CHAR *pcDest, const CHAR *pcSearch)
	{
		if (pcDest == NULL || *pcDest == NULL || pcSearch == NULL || *pcSearch == NULL)
		{
			return -1;
		}

		INT iDestLen = strlen(pcDest);
		INT iSearchLen = strlen(pcSearch);

		if (iDestLen < iSearchLen)
		{
			return -1;
		}
		
		const CHAR *pcTail = pcDest + (iDestLen - iSearchLen);
		const CHAR *pcCur = pcTail;

		while (pcCur >= pcDest)
		{
			if (strncmp(pcCur, pcSearch, iSearchLen))
			{
				return (pcCur - pcDest);
			}

			pcCur--;
		}

		return -1;
	}

	BOOLEAN String::strstart(const CHAR* pcSrc, const CHAR *pcSub)
	{
		if (pcSrc == NULL || *pcSrc == NULL || pcSub == NULL || *pcSub == NULL)
		{
			return FALSE;
		}

		INT iSrcLen = strlen(pcSrc);
		INT iSubLen = strlen(pcSub);

		if (iSrcLen < iSubLen)
		{
			return FALSE;
		}
		
		return strncmp(pcSrc, pcSub, iSubLen);
	}

	BOOLEAN String::strend(const CHAR* pcSrc, const CHAR* pcSub)
	{
		if (pcSrc == NULL || *pcSrc == NULL || pcSub == NULL || *pcSub == NULL)
		{
			return FALSE;
		}

		INT iSrcLen = strlen(pcSrc);
		INT iSubLen = strlen(pcSub);

		if (iSrcLen < iSubLen)
		{
			return FALSE;
		}
		
		return strncmp(pcSrc + (iSrcLen - iSubLen), pcSub, iSubLen);
	}

	INT	String::strcmp(const CHAR *pcSrc1, const CHAR *pcSrc2)
	{
		if (pcSrc1 == NULL && pcSrc2 == NULL)
		{
			return 0;
		}
		
		if (pcSrc1 != NULL && pcSrc2 == NULL)
		{
			return 1;
		}

		if (pcSrc1 == NULL && pcSrc2 != NULL)
		{
			return -1;
		}

		for ( ; ; pcSrc1++, pcSrc2++)
		{
			if (*pcSrc1 == NULL)
			{
				if (*pcSrc2 == NULL)
				{
					return 0;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				if (*pcSrc2 == NULL)
				{
					return 1;
				}
				else
				{
					if (*pcSrc1 > *pcSrc2)
					{
						return 1;
					}
					else if (*pcSrc1 < *pcSrc2)
					{
						return -1;
					}
					else
					{
						continue;
					}
				}
			}
		}

		return 0;
	}

	BOOLEAN String::strncmp(const CHAR *pcSrc1, const CHAR *pcSrc2, INT iLen)
	{
		INT i = 0;
		while (i < iLen)
		{
			if (*(pcSrc1 + i) != *(pcSrc2 + i) )
			{
				return FALSE;
			}
			i++;
		}

		return TRUE;
	}

	void String::strcat(CHAR *pcDest, const CHAR *pcSrc)
	{
		if (pcDest == NULL || pcSrc == NULL)
		{
			return;
		}

		CHAR *pCur = pcDest + strlen(pcDest);

		while (*pcSrc != NULL)
		{
			*pCur++ = *pcSrc++;
		}

		*pCur = NULL;
	}

	void String::strcpy(CHAR *pcDest, const CHAR *pcSrc)
	{
		if (pcDest == NULL || pcSrc == NULL)
		{
			return;
		}

		while (*pcSrc != NULL)
		{
			*pcDest++ = *pcSrc++;
		}

		*pcDest = NULL;
	}

	void String::strncpy(CHAR *pcDest, const CHAR *pcSrc, UINT uiLen)
	{
		if (pcDest == NULL || pcSrc == NULL)
		{
			return;
		}

		UINT i = 0;
		while (*pcSrc != NULL && i < uiLen)
		{
			*pcDest++ = *pcSrc++;
			i++;
		}

		*pcDest = NULL;
	}

#ifdef OS_WIN
	void String::printf(const CHAR *pcFormat, ...)
	{
		va_list argList;
		va_start(argList, pcFormat);
		vwprintf(pcFormat, argList);
		va_end(argList);
	}

	String String::format(const CHAR *pcFormat, ...)
	{
		CHAR acTemp[1024] = {0};

		va_list argList;
		va_start(argList, pcFormat);
		vswprintf_s(acTemp, 1024, pcFormat, argList);
		va_end(argList);

		return acTemp;
	}
#endif

#ifdef OS_QNX
	void String::printf(const CHAR *pcFormat, ...)
	{
		va_list argList;
		va_start(argList, pcFormat);
		vwprintf(pcFormat, argList);
		va_end(argList);
	}

	String String::format(const CHAR *pcFormat, ...)
	{
		CHAR acTemp[1024] = {0};

		va_list argList;
		va_start(argList, pcFormat);
		vswprintf(acTemp, 1024, pcFormat, argList);
		va_end(argList);

		return acTemp;
	}	
#endif

#ifdef OS_LINUX
	void String::printf(const CHAR *pcFormat, ...)
	{
		va_list argList;
		va_start(argList, pcFormat);
		vwprintf(pcFormat, argList);
		va_end(argList);
	}

	String String::format(const CHAR *pcFormat, ...)
	{
		CHAR acTemp[1024] = {0};

		va_list argList;
		va_start(argList, pcFormat);
		vswprintf(acTemp, 1024, pcFormat, argList);
		va_end(argList);

		return acTemp;
	}
#endif	

	void String::convertFromBYTE_UTF16(const BYTE *pbtData, UINT uiLen, String &strOut)
	{
		if (pbtData == NULL || uiLen == 0)
		{
			return;
		}

		const USHORT *pstData = reinterpret_cast<const USHORT*>(pbtData);

		UINT uiNewLen = uiLen / 2;

		CHAR *pcData = new CHAR[uiNewLen + 1];

		for (UINT i = 0; i < uiNewLen; i++)
		{
			pcData[i] = pstData[i];
		}
		pcData[uiNewLen] = NULL;

		strOut.free();
		strOut.m_pc = pcData;
		strOut.m_uiLen = uiNewLen;
	}

#ifdef OS_WIN
	void String::convertToMBCS(const CHAR *pcSrc, char *pcOut, UINT uiBufferLen)
	{
		if (pcSrc == NULL || pcOut == NULL || uiBufferLen == 0)
		{
			return;
		}

		char acSysLocalCharset[32] = {0};
		innerConvertToMBCS(CONFIG::getSysLocalCharset(), acSysLocalCharset, 32);

		setlocale(LC_ALL,acSysLocalCharset);

		UINT uiSize = 0;
		wcstombs_s(&uiSize, pcOut, uiBufferLen, pcSrc, uiBufferLen - 1);
	}

	void String::convertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen)
	{
		if (strSrc.m_pc == NULL || pcOut == NULL || uiBufferLen == 0)
		{
			return;
		}

		char acSysLocalCharset[32] = {0};
		innerConvertToMBCS(CONFIG::getSysLocalCharset(), acSysLocalCharset, 32);

		setlocale(LC_ALL,acSysLocalCharset);

		UINT uiSize = 0;
		wcstombs_s(&uiSize, pcOut, uiBufferLen, strSrc.m_pc, uiBufferLen - 1);
	}

	void String::innerConvertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen)
	{
		if (strSrc.m_pc == NULL)
		{
			return;
		}

		UINT uiSize = 0;
		wcstombs_s(&uiSize, pcOut, uiBufferLen, strSrc.m_pc, uiBufferLen);
	}
#endif

#ifdef OS_QNX
	void String::convertToMBCS(const CHAR *pcSrc, char *pcOut, UINT uiBufferLen)
	{
		if (pcSrc == NULL || pcOut == NULL || uiBufferLen == 0)
		{
			return;
		}

		wcstombs(pcOut, pcSrc, uiBufferLen - 1);
	}

	void String::convertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen)
	{
		if (strSrc.m_pc == NULL || pcOut == NULL || uiBufferLen == 0)
		{
			return;
		}

		wcstombs(pcOut, strSrc.m_pc, uiBufferLen - 1);
	}

	void String::innerConvertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen)
	{
		if (strSrc.m_pc == NULL)
		{
			return;
		}

		wcstombs(pcOut, strSrc.m_pc, uiBufferLen);
	}
#endif

#ifdef OS_LINUX
	void String::convertToMBCS(const CHAR *pcSrc, char *pcOut, UINT uiBufferLen)
	{
		if (pcSrc == NULL || pcOut == NULL || uiBufferLen == 0)
		{
			return;
		}

		wcstombs(pcOut, pcSrc, uiBufferLen - 1);
	}

	void String::convertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen)
	{
		if (strSrc.m_pc == NULL || pcOut == NULL || uiBufferLen == 0)
		{
			return;
		}

		wcstombs(pcOut, strSrc.m_pc, uiBufferLen - 1);
	}

	void String::innerConvertToMBCS(const String &strSrc, char *pcOut, UINT uiBufferLen)
	{
		if (strSrc.m_pc == NULL)
		{
			return;
		}

		wcstombs(pcOut, strSrc.m_pc, uiBufferLen);
	}
#endif	

	void String::convertFromMBCS(const char *pcSrc, String &strOut)
	{
		if (pcSrc == NULL)
		{
			strOut = STRING_NULL;
			return;
		}

		CHAR *pcData = NULL;

#ifdef OS_WIN
		UINT uiLen = ::strlen(pcSrc);
		pcData = new CHAR[uiLen + 1];
		memset(pcData, 0, sizeof(CHAR) * (uiLen + 1));

		UINT uiSize = 0;
		mbstowcs_s(&uiSize, pcData, uiLen + 1, pcSrc, uiLen);
#endif

#ifdef OS_QNX
		UINT uiLen = ::strlen(pcSrc);
		pcData = new CHAR[uiLen + 1];
		memset(pcData, 0, sizeof(CHAR) * (uiLen + 1));

		mbstowcs(pcData, pcSrc, uiLen);
#endif

#ifdef OS_LINUX
		UINT uiLen = ::strlen(pcSrc);
		pcData = new CHAR[uiLen + 1];
		memset(pcData, 0, sizeof(CHAR) * (uiLen + 1));

		mbstowcs(pcData, pcSrc, uiLen);
#endif

		strOut.free();
		strOut.m_pc = pcData;
		strOut.m_uiLen = String::strlen(pcData);
	}

	void String::convertToUTF8(const CHAR *pcSrc, BYTE *pbtOut, UINT uiBufferLen, UINT *puiOutLen)
	{
		if (pcSrc == NULL || pbtOut == NULL || uiBufferLen == 0)
		{
			if (puiOutLen != NULL)
			{
				*puiOutLen = 0;
			}
			return;
		}

		UINT uiCurPos = 0;
		UINT uiTailIndex = uiBufferLen - 1;

		while (*pcSrc != NULL)
		{
			if (*pcSrc <= 0x007F)
			{
				if (uiCurPos >= uiTailIndex)
				{
					break;
				}

				*pbtOut++ = (BYTE)(*pcSrc & 0x00FF);

				uiCurPos += 1;
			}
			else if (0x0080 <= *pcSrc && *pcSrc <= 0x07FF)
			{
				if (uiCurPos + 1 >= uiTailIndex)
				{
					break;
				}

				*pbtOut++ = (BYTE)(((*pcSrc >> 6) & 0x001F) | 0x00C0);
				*pbtOut++ = (BYTE)((*pcSrc & 0x003F) | 0x0080);

				uiCurPos += 2;
			}
			else // *pcSrc >= 0x0800
			{
				if (uiCurPos + 2 >= uiTailIndex)
				{
					break;
				}

				*pbtOut++ = (BYTE)(((*pcSrc >> 12) & 0x000F) | 0x00E0);
				*pbtOut++ = (BYTE)(((*pcSrc >> 6) & 0x003F) | 0x0080);
				*pbtOut++ = (BYTE)((*pcSrc & 0x003F) | 0x0080);

				uiCurPos += 3;
			}

			pcSrc++;
		}

		pbtOut[uiCurPos] = NULL;

		if (puiOutLen != NULL)
		{
			*puiOutLen = uiCurPos;
		}
	}

	void String::convertToUTF8(const String &strSrc, BYTE *pbtOut, UINT uiBufferLen, UINT *puiOutLen)
	{
		convertToUTF8(strSrc.m_pc, pbtOut, uiBufferLen, puiOutLen);
	}

	void String::convertFromUTF8(const BYTE *pbtSrc, String &strOut)
	{
		if (pbtSrc == NULL)
		{
			strOut = STRING_NULL;
			return;
		}

		UINT uiLen = ::strlen((char*)pbtSrc);
		CHAR *pcData = new CHAR[uiLen + 1];
		memset(pcData, 0, sizeof(CHAR) * (uiLen + 1));

		CHAR *pcCurPos = pcData;

		while (*pbtSrc != NULL)
		{
			if (((*pbtSrc) & 0xE0) == 0xE0)
			{
				if (*(pbtSrc + 1) == NULL || *(pbtSrc + 2) == NULL)
				{
					break;
				}

				*pcCurPos++ =  (((*pbtSrc) & 0x0F) << 12)
					| (((*(pbtSrc + 1)) & 0x3F) << 6)
					| (((*(pbtSrc + 2)) & 0x3F));

				pbtSrc += 3;
			}
			else if (((*pbtSrc) & 0xC0) == 0xC0)
			{
				if (*(pbtSrc + 1) == NULL)
				{
					break;
				}

				*pcCurPos++ =  (((*pbtSrc) & 0x1F) << 6)
							 | (((*(pbtSrc + 1)) & 0x3F));

				pbtSrc += 2;
			}
			else if (((*pbtSrc) & 0x80) == 0)
			{
				*pcCurPos++ = *pbtSrc;

				pbtSrc += 1;
			}
			else
			{
				pbtSrc += 1;
			}
		}

		strOut.free();
		strOut.m_pc = pcData;
		strOut.m_uiLen = pcCurPos - pcData;
	}

	void String::encodeChars(const String &strSrc, UINT uiCharCount, const CHAR acCharTab[], const CHAR* apcEncStrTab[], String &strOut) 
	{
		UINT uiSrcLen = strSrc.length();
		if (uiSrcLen == 0)
		{
			strOut = STRING_NULL;
			return;
		}

		// Calculate buffer size requested.
		BOOLEAN bFound;
		UINT uiDestLen = 0;
		for (UINT i = 0; i < uiSrcLen; i++)
		{
			bFound = FALSE;
			for (UINT j = 0; j < uiCharCount; j++)
			{
				if (strSrc.charAt(i) == acCharTab[j])
				{
					uiDestLen += strlen(apcEncStrTab[j]);
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
			{
				continue;
			}

			++uiDestLen;
		}

		CHAR* pcBuffer = new CHAR[uiDestLen + sizeof(CHAR)];
		pcBuffer[uiDestLen] = L'\0';
		UINT uiBufPos = 0;

		for (UINT i = 0; i < uiSrcLen; i++)
		{
			bFound = FALSE;
			for (UINT j = 0; j < uiCharCount; j++)
			{
				if (strSrc.charAt(i) == acCharTab[j])
				{
					UINT uiStrEncLen = strlen(apcEncStrTab[j]);
					for (UINT k = 0; k < uiStrEncLen; k++)
					{
						pcBuffer[uiBufPos] = apcEncStrTab[j][k];
						++uiBufPos;
					}
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
			{
				continue;
			}

			pcBuffer[uiBufPos] = strSrc.charAt(i);
			++uiBufPos;
		}

		strOut = pcBuffer;
		delete[] pcBuffer;
	}

	void String::decodeChars(const String &strSrc, UINT uiCharCount, const CHAR acCharTab[], const CHAR* apcEncStrTab[], String &strOut)
	{
		UINT uiSrcLen = strSrc.length();
		if (uiSrcLen == 0)
		{
			strOut = STRING_NULL;
			return;
		}

		CHAR* pcBuffer = new CHAR[uiSrcLen + sizeof(CHAR)];
		const CHAR* pcSrcStr = (const CHAR*)(strSrc);
		const CHAR* pcEndPos = pcSrcStr + uiSrcLen;
		UINT uiBufPos = 0;
		BOOLEAN bFound;

		while (pcSrcStr <= pcEndPos)
		{
			bFound = FALSE;
			for (UINT j = 0; j < uiCharCount; j++)
			{
				UINT uiStrEncLen = strlen(apcEncStrTab[j]);
				
				if ((pcSrcStr <= pcEndPos - uiStrEncLen) && 
					strncmp(pcSrcStr, apcEncStrTab[j], uiStrEncLen))
				{
					pcBuffer[uiBufPos] = acCharTab[j];
					pcSrcStr += uiStrEncLen;
					++uiBufPos;
					bFound = TRUE;
					break;
				}
			}
			if (bFound)
			{
				continue;
			}

			pcBuffer[uiBufPos] = *pcSrcStr;
			++pcSrcStr;
			++uiBufPos;
		}

		pcBuffer[uiBufPos] = L'\0';
		strOut = pcBuffer;
		delete[] pcBuffer;
	}

	String::String() 
		: m_pc(NULL)
		, m_uiLen(0)
	{
		;
	}

	String::String(const CHAR *pcSrc) 
		: m_pc(NULL)
		, m_uiLen(0)
	{
		duplicate(pcSrc);
	}

	String::String(const CHAR *pcSrc, UINT uiLen)
		: m_pc(NULL)
		, m_uiLen(0)
	{
		UINT uiSrcLen = strlen(pcSrc);
		if ((uiSrcLen == 0) || (uiLen == 0))
		{
			return;
		}

		if (uiLen > uiSrcLen)
		{
			uiLen = uiSrcLen;
		}

		m_uiLen = uiLen;
		m_pc = new CHAR[m_uiLen + 1];
		strncpy(m_pc, pcSrc, m_uiLen);
	}

	String::String(const String &strSrc)
		: m_pc(NULL)
		, m_uiLen(0)
	{
		duplicate(strSrc.m_pc);
	}

	String::~String()
	{
		free();
	}

	UINT String::length() const
	{
		return m_uiLen;
	}

	CHAR String::charAt(UINT uiIndex) const
	{
		if (uiIndex >= m_uiLen)
		{
			return NULL;
		}

		return m_pc[uiIndex];
	}

	INT String::indexOf(const CHAR *pcSubString, UINT uiStartIndex) const
	{
		INT iPos = strstr(m_pc + uiStartIndex, pcSubString);
		return iPos < 0 ? iPos : iPos + uiStartIndex;
	}

	INT String::indexOf(const String &strSubString, UINT uiStartIndex) const
	{
		INT iPos = strstr(m_pc + uiStartIndex, strSubString.m_pc);
		return iPos < 0 ? iPos : iPos + uiStartIndex;
	}

	INT String::lastIndexOf(const CHAR *pcSubString) const
	{
		return strstr_last(m_pc, pcSubString);
	}

	INT String::lastIndexOf(const String &strSubString) const
	{
		return strstr_last(m_pc, strSubString.m_pc);
	}

	INT String::compare(const CHAR *pcSrc) const
	{
		return strcmp(m_pc, pcSrc);
	}

	INT String::compare(const String &strSrc) const
	{
		return strcmp(m_pc, strSrc.m_pc);
	}

	String&	String::append(const CHAR *pcSrc)
	{
		UINT uiLen = strlen(pcSrc);
		if (uiLen == 0)
		{
			return *this;
		}

		CHAR *pcTemp = new CHAR[m_uiLen + uiLen + 1];
		strcpy(pcTemp, m_pc);
		strcpy(pcTemp + m_uiLen, pcSrc);
		uiLen = m_uiLen + uiLen;
		
		free();

		m_pc = pcTemp;
		m_uiLen = uiLen;

		return *this;
	}

	String String::subString(UINT uiStartIndex, UINT uiLen) const
	{
		UINT uiCopyLen = 0;
		if (uiStartIndex >= m_uiLen)
		{
			uiCopyLen = 0;
		}
		else
		{
			uiCopyLen = m_uiLen - uiStartIndex;

			if (0 < uiLen && uiLen < uiCopyLen)
			{
				uiCopyLen = uiLen;
			}
		}

		CHAR *pcTemp = new CHAR[uiCopyLen + 1];
		strncpy(pcTemp, m_pc + uiStartIndex, uiCopyLen);
		
		String str;
		str.m_pc = pcTemp;
		str.m_uiLen = uiCopyLen;

		return str;
	}

	void String::replace(const CHAR cOld, const CHAR cNew)
	{
		if (m_pc == NULL)
		{
			return;
		}

		CHAR *pcTemp = m_pc;

		while (*pcTemp != NULL)
		{
			if (*pcTemp == cOld)
			{
				*pcTemp = cNew;
			}

			pcTemp++;
		}
	}

	void String::replaceFirst(const CHAR *pcOld, const CHAR *pcNew)
	{
		INT iIndex = indexOf(pcOld);
		if (iIndex < 0)
		{
			return;
		}

		INT iSearchLen = strlen(pcOld);
		INT iReplaceLen = strlen(pcNew);

		CHAR *pcTemp = new CHAR[m_uiLen + iReplaceLen - iSearchLen + 1];

		strncpy(pcTemp, m_pc, iIndex);
		strncpy(pcTemp + iIndex, pcNew, iReplaceLen);
		strncpy(pcTemp + iIndex + iReplaceLen,
				m_pc + iIndex + iSearchLen,
				m_uiLen - iIndex - iSearchLen);

		free();

		m_pc = pcTemp;
		m_uiLen = strlen(m_pc);

		/*
		if (m_pc == NULL || *m_pc == NULL || pcOld == NULL || *pcOld == NULL || pcNew == NULL)
		{
			return;
		}

		INT iSearchLen = strlen(pcOld);
		INT iReplaceLen = strlen(pcNew);

		if (m_uiLen < iSearchLen)
		{
			return;
		}

		ArrayPtr<INT> ptrInt(new INT[m_uiLen]);

		const CHAR *pcCur = m_pc;
		const CHAR *pcTail = m_pc + (m_uiLen- iSearchLen);

		INT iCount = 0;
		while (pcCur <= pcTail)
		{
			if (strncmp(pcCur, pcOld, iSearchLen))
			{
				ptrInt[iCount++] = pcCur - m_pc;
				pcCur += iSearchLen;
			}
			else
			{
				pcCur++;
			}
		}

		if (iCount == 0)
		{
			return;
		}

		CHAR *pcTemp = new CHAR[m_uiLen + iCount * iReplaceLen];

		strncpy(pcTemp, m_pc, ptrInt[0]);
		INT iCurLen = ptrInt[0];
		for ()
		{
			strncpy(pcTemp + iCurLen, pcNew, iReplaceLen);
			iCurLen += iReplaceLen;
		}

		free();

		m_pc = pcTemp;
		m_uiLen = iCurLen;
		*/
	}

	void String::toUpperCase()
	{
		for (UINT uiCnt = 0; uiCnt < m_uiLen; uiCnt++)
		{
			if( m_pc[uiCnt] >= L'a' && m_pc[uiCnt] <= L'z' )
			{
				m_pc[uiCnt] -= 32;
			}
		}
	}

	void String::toLowerCase()
	{
		for (UINT uiCnt = 0; uiCnt < m_uiLen; uiCnt++)
		{
			if( m_pc[uiCnt] >= L'A' && m_pc[uiCnt] <= L'Z' )
			{
				m_pc[uiCnt] += 32;
			}
		}
	}

	String& String::operator =(const CHAR *pcSrc)
	{
		if (m_pc != pcSrc)
		{
			free();
			duplicate(pcSrc);
		}

		return *this;
	}

	String& String::operator =(const String &strSrc)
	{
		free();
		duplicate(strSrc.m_pc);

		return *this;
	}

	String String::operator +(const CHAR *pcSrc) const
	{
		UINT uiLen = strlen(pcSrc);

		CHAR *pcTemp = new CHAR[m_uiLen + uiLen + 1];
		strcpy(pcTemp, m_pc);
		strcpy(pcTemp + m_uiLen, pcSrc);

		String str;
		str.m_pc = pcTemp;
		str.m_uiLen = m_uiLen + uiLen;

		return str;
	}

	String String::operator +(const String &strSrc) const
	{
		UINT uiLen = strSrc.length();

		CHAR *pcTemp = new CHAR[m_uiLen + uiLen + 1];
		strcpy(pcTemp, m_pc);
		strcpy(pcTemp + m_uiLen, strSrc.m_pc);

		String str;
		str.m_pc = pcTemp;
		str.m_uiLen = m_uiLen + uiLen;

		return str;
	}

	CHAR String::operator [](UINT uiIndex) const
	{
		return charAt(uiIndex);
	}

	BOOLEAN String::operator ==(const CHAR *pcSrc) const
	{
		return strcmp(m_pc, pcSrc) == 0 ? TRUE : FALSE;
	}

	BOOLEAN String::operator ==(const String &strSrc) const
	{
		return strcmp(m_pc, strSrc.m_pc) == 0 ? TRUE : FALSE;
	}

	BOOLEAN String::operator !=(const CHAR *pcSrc) const
	{
		return strcmp(m_pc, pcSrc) != 0 ? TRUE : FALSE;
	}

	BOOLEAN String::operator !=(const String &strSrc) const
	{
		return strcmp(m_pc, strSrc.m_pc) != 0 ? TRUE : FALSE;
	}

	void String::duplicate(const CHAR *pcSrc)
	{
		if (pcSrc != NULL)
		{
			m_uiLen = strlen(pcSrc);

			m_pc = new CHAR[m_uiLen + 1];

			strcpy(m_pc, pcSrc);
		}
	}

	void String::free()
	{
		if (m_pc !=	NULL)
		{
			delete [] m_pc;
			m_pc = NULL;

			m_uiLen = 0;
		}
	}
}
