/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

#ifdef OS_WIN
	#include <stdio.h>
#endif

#ifdef OS_QNX
	#include <stdio.h>
#endif

#ifdef OS_LINUX
	#include <stdio.h>
#endif

namespace OpenHMI
{
	/**
	* @class	CFile	
	*			It is a common file class.
	*/
	class OPENHMI_API CFile : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ESeekType
		{
			ST_BEGIN = 0,
			ST_CURRENT = 1,
			ST_END = 2,
		};

	private:// embed class or struct or enum

	public:// method
		CFile();
		virtual ~CFile();

		static	BOOLEAN isExist(const CHAR * pcFilePath);
		static	BOOLEAN	isReadOnly(const CHAR * pcFilePath);

		BOOLEAN	open(const CHAR *pcFilePath, BOOLEAN bRead);
		void	close();

		BOOLEAN	isEmpty();		
		UINT	getFileSize();

		BOOLEAN	writeData(const BYTE *pbtData, UINT uiDataLen);
		BOOLEAN	readData(BYTE *pbtData, UINT uiDataLen);
		void	seekTo(INT iDistance, ESeekType eSeekType);


	protected:// method
		CFile(const CFile&){}
		CFile& operator =(const CFile&){return *this;}

		FILE * innerOpen(const CHAR *pcFilePath, const CHAR *pcMode);

	private:// method

	protected:// property

	private:// property
		FILE		*m_pFile;
	};
}