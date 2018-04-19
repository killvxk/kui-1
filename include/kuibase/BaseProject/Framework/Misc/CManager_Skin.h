/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CManager_Skin : public IMovieClipGenerator
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		struct tagSkinData
		{
			String	strID;
			String	strName;
		};

	private:// embed class or struct or enum

	public:// method	
		static	CManager_Skin*	getInstance();
		static	void			deleteInstance();

		/**
		*  Inner method.
		*/
		void		parseConfig();

		/**
		*  Inner method.
		*/
		void		initialize();

		/**
		*  Inner method.
		*/
		void		unInitialize();

		/**
		* Get the number of installed skin in the system.
		*/
		UINT		getSkinCount();	

		/**
		* Get skin ID and name for the pointed skin by index.
		*/
		tagSkinData	getSkin(UINT uiIndex);

		/**
		* @brief	Active a skin specified by skin ID which is defined in the Config.ini file. 
		*
		* @param	strID	The ID used to specify the skin.
		*/
		void		setActiveSkin(const String &strID);

		/**
		* Get the current active skin ID.
		*/
		String		getActiveSkin()	{ return m_strActiveSkin; }

		/**
		*  Inner method.
		*/
		void		notifyLanguageChanged();

		/**
		*  Inner method.
		*/
		virtual CMovieClip*	createMovieClip(UINT uiImageID);

	protected:// method
		CManager_Skin();
		virtual ~CManager_Skin();

	private:// method
		CConfigParser::tagSkin* getSkinByID(const String &strID);
		void					loadSkin(CConfigParser::tagSkin *pSkin);

	protected:// property

	private:// property
		static	CManager_Skin		*s_pInstance;

		CConfigParser::tagSkinInfo	*m_pSkinInfo;
		String						m_strActiveSkin;
		String						m_strNewActiveSkin;
	};
}