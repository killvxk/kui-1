/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CPath	
	*			It is a common path class.
	*/
	class OPENHMI_API CPath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	BOOLEAN	getFileFolder(const CHAR *pcFilePath,  CHAR *pcFileFolder);
		static  String  getFileFolder(const CHAR *pcFilePath);
		static	BOOLEAN	getFileName(const CHAR *pcFilePath, CHAR *pcFileName);
		static  String  getFileName(const CHAR *pcFilePath);
		static	BOOLEAN getFileExtern(const CHAR *pcFilePath, CHAR *pcFileExtern);
		static  String  getFileExtern(const CHAR *pcFilePath);
		static	String  changeFileExtern(const CHAR *pcFileName, CHAR* pcNewExtern);

		static	String	getApplicationFolder();
		static	String	generateRelativePath(const CHAR *pcPathParent, BOOLEAN bIsDirectory, const CHAR *pcPathChild);
		static	String	generateAbsolutePath(const CHAR *pcPathParent, const CHAR *pcPathChild);

	protected:// method
		CPath(){}		
		CPath(const CPath&){}
		virtual ~CPath(){}
		CPath& operator =(const CPath&){return *this;}

	private:// method
		static	INT		getFileNameStartIndex(const CHAR *pcFilePath);
		
	protected:// property

	private:// property

	};
}