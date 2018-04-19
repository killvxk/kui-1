/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CONFIG
	{
	public:// const define
		static	const UINT		CONTAINER_TYPE_SCREEN		= 1;
		static	const UINT		CONTAINER_TYPE_POPUP		= 2;
		static	const UINT		CONTAINER_TYPE_FUNCWIDGET	= 3;
		static	const UINT		CONTAINER_TYPE_CTRLWIDGET	= 4;

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Enable or disable the CCP feature in the system.
		*/
		static	void	setUseCCP(BOOLEAN bValue)				{ s_bUseCCP = bValue; }
		static	BOOLEAN	getUseCCP()								{ return s_bUseCCP; }	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Check whether run on target.
		*/
		static	void	checkRunOnTarget();

		static	BOOLEAN	isRunOnTarget()							{ return s_bRunOnTarget; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	It is a wrapper for temp code. Example:
		*
		*			if (CONFIG::useTempCode())
		*			{ write your temp code here. }
		*
		* @return	TRUE Means the system is running temp code. FALSE Means the system is running formal code.
		*/
		static	BOOLEAN	useTempCode()							{ return CConfigParser::getInstance()->getSysInfo()->bUseTempCode; }
		
		static	BOOLEAN	getOutputPerformance()					{ return CConfigParser::getInstance()->getSysInfo()->bOutputPerformance; }		
		static	BOOLEAN	getOutputErrorLog()						{ return CConfigParser::getInstance()->getSysInfo()->bOutputErrorLog; }
		static	BOOLEAN	getOutputWarningLog()					{ return CConfigParser::getInstance()->getSysInfo()->bOutputWarningLog; }		
		static	BOOLEAN	getOutputInfoLog()						{ return CConfigParser::getInstance()->getSysInfo()->bOutputInfoLog; }
		static	BOOLEAN	getOutputDebugLog()						{ return CConfigParser::getInstance()->getSysInfo()->bOutputDebugLog; }		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static	UINT	getCharacterCacheCount()				{ return CConfigParser::getInstance()->getSysInfo()->uiCharacterCacheCount; }
		static	UINT	getImageCacheCount()					{ return CConfigParser::getInstance()->getSysInfo()->uiImageCacheCount; }
		static	UINT	getTextureCacheCount()					{ return CConfigParser::getInstance()->getSysInfo()->uiTextureCacheCount; }
		static	UINT	getProgramCacheCount()					{ return CConfigParser::getInstance()->getSysInfo()->uiProgramCacheCount; }
		static	UINT	getShaderCacheCount()					{ return CConfigParser::getInstance()->getSysInfo()->uiShaderCacheCount; }
		static	UINT	getMaterialCacheCount()					{ return CConfigParser::getInstance()->getSysInfo()->uiMaterialCacheCount; }
		static	UINT	getMeshCacheCount()						{ return CConfigParser::getInstance()->getSysInfo()->uiMeshCacheCount; }

		static	const String&	getSysLocalCharset()			{ return CConfigParser::getInstance()->getSysInfo()->strSysLocalCharset; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		static	String	getSvcIpcHost()							{ return CConfigParser::getInstance()->getSvcIpcInfo()->strHost; }
		static	UINT	getSvcIpcPort()							{ return CConfigParser::getInstance()->getSvcIpcInfo()->uiPort; }

	protected:// method
		CONFIG(){}
		CONFIG(const CONFIG&){}
		virtual ~CONFIG(){}		
		CONFIG& operator =(const CONFIG&){return *this;}

	private:// method

	protected:// property

	private:// property		
		static	BOOLEAN		s_bUseCCP;
		static	BOOLEAN		s_bRunOnTarget;
	};
}