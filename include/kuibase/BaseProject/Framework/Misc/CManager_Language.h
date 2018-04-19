/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CManager_Language
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		struct tagLanguageData
		{
			String	strID;
			String	strName;
		};

	private:// embed class or struct or enum
	
	public:// method	
		static	CManager_Language*	getInstance();	
		static	void				deleteInstance();	

		/**
		*  Inner method.
		*/
		void			parseConfig();

		/**
		*  Inner method.
		*/
		void			initialize();

		/**
		*  Inner method.
		*/
		void			unInitialize();

		/**
		* Get the number of installed language in the system.
		*/
		UINT			getLanguageCount();

		/**
		* Get language ID and name for the pointed language by index.
		*/
		tagLanguageData	getLanguage(UINT uiIndex);

		/**
		* @brief	Active a language specified by language ID which is defined in the Config.ini file. 
		*
		* @param	strID	The ID used to specify the language.
		*/
		void			setActiveLanguage(const String &strID);

		/**
		* Get the current active language ID.
		*/
		const String&	getActiveLanguage()	{ return m_strActiveLanguage; }

	protected:// method
		CManager_Language();
		virtual ~CManager_Language();

	private:// method
		CConfigParser::tagLanguage* getLanguageByID(const String &strID);

	protected:// property

	private:// property
		static	CManager_Language		*s_pInstance;

		CConfigParser::tagLanguageInfo	*m_pLanguageInfo;
		String							m_strActiveLanguage;
	};
}