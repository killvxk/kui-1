/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CManager_Service
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	CManager_Service*	getInstance();	
		static	void				deleteInstance();

		/**
		*  Inner method.
		*/
		void			initialize();

		/**
		*  Inner method.
		*/
		void			unInitialize();

		/**
		* @brief	Register a new service class to CManager_Service.
		*
		* @param	strClassName	A name used to indicate a service class.
		* @param	pLogic			The pointer of a service class.
		*/
		void			registerService(const String &strClassName, CBaseService *pService);

		/**
		*  Inner method.
		*/
		CBaseService*	getServiceByName(const String &strClassName);		

	protected:// method
		CManager_Service();
		virtual ~CManager_Service();	
		
	private:// method

	protected:// property

	private:// property
		static	CManager_Service	*s_pInstance;

		Map<String, CBaseService, tagStringCompare>		m_mapService;
	};
}