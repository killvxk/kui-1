/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CManager_Application
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static	CManager_Application*	getInstance();	
		static	void					deleteInstance();	

		/**
		*  Inner method.
		*/
		void				initialize();

		/**
		*  Inner method.
		*/
		void				unInitialize();

		/**
		* @brief	Register a new application class to CManager_Application.
		*
		* @param	strClassName	A name used to indicate a application class.
		* @param	pApplication	The pointer of a application class.
		*/
		void				registerApplication(const String &strClassName, CBaseApplication *pApplication);

		/**
		*  Inner method.
		*/
		CBaseApplication*	getApplicationByName(const String &strClassName);

		/**
		*  Inner method.
		*/
		void	startApplication(const String &strClassName, const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand);

		/**
		*  Inner method.
		*/
		void	closeApplication(const String &strClassName);

		/**
		*  Inner method.
		*/
		void	closeAllApplications();

	protected:// method
		CManager_Application();
		virtual ~CManager_Application();

	private:// method

	protected:// property

	private:// property
		static	CManager_Application	*s_pInstance;

		Map<String, CBaseApplication, tagStringCompare>		m_mapApplication;
	};
}