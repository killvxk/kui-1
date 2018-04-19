/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CLog	
	*			This class is used to output error, warning ,info and debug information to terminal.
	*			And there is a switch in the Config.ini file to contrl ouput or not.
	*/
	class OPENHMI_API CLog
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Write error description.
		*/
		static void error2(const String &strTab, const String &strMsg);

		/**
		* Write error description.
		*/
		static void error2(const CHAR *pcTab, const CHAR *pcMsg);
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Write warning description.
		*/
		static void warning2(const String &strTab, const String &strMsg);

		/**
		* Write warning description.
		*/
		static void warning2(const CHAR *pcTab, const CHAR *pcMsg);
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Write info description.
		*/
		static void info(const String &strTab, const String &strMsg);

		/**
		* Write info description.
		*/
		static void info(const CHAR *pcTab, const CHAR *pcMsg);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Write debug description.
		*/
		static void debug2(const String &strTab, const String &strMsg);

		/**
		* Write debug description.
		*/
		static void debug2(const CHAR *pcTab, const CHAR *pcMsg);
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Set flag in temp code place. The temp code will be deleted in the future.
		* @param	pcClassName		The class name of which temp codes in.
		* @param	pcFunctionName	The function name of which temp codes in.
		*/
		static void tempCodePos(const CHAR *pcClassName, const CHAR *pcFunctionName);

	protected:// method
		CLog(){}
		CLog(const CLog&){}
		virtual ~CLog(){}		
		CLog& operator =(const CLog&){return *this;}		

	private:// method
		static void writeLog(const CHAR *pcType, const String &strTab, const String &strMsg);
		static void writeLog(const CHAR *pcType, const CHAR *pcTab, const CHAR *pcMsg);

	protected:// property

	private:// property

	};
}