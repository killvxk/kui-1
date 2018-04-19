/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CConfigParser
	{
	public:// const define

	private:// const define
		
	public:// embed class or struct or enum
		struct tagSysInfo
		{
			BOOLEAN bUseTempCode;
			BOOLEAN bNeedTray;

			BOOLEAN	bOutputPerformance;
			BOOLEAN bOutputErrorLog;
			BOOLEAN bOutputWarningLog;
			BOOLEAN bOutputInfoLog;
			BOOLEAN bOutputDebugLog;

			UINT	uiCharacterCacheCount;
			UINT	uiImageCacheCount;
			UINT	uiTextureCacheCount;
			UINT	uiProgramCacheCount;
			UINT	uiShaderCacheCount;
			UINT	uiMaterialCacheCount;
			UINT	uiMeshCacheCount;
			UINT	uiWebBrowserCount;
			UINT	uiEditCtrlCount;

			String	strSysLocalCharset;

			INT		iMsgQueueThreadPri;
			INT		iScreenTouchThreadPri;
			INT		iTimerThreadPri;
			INT		iServiceAdapterThreadPri;
			INT		iTuioWrapperThreadPri;
		};

		struct tagTextRenderOptInfo
		{
			UINT	uiWidth;
			UINT	uiHeight;
			UINT	uiCellWidth;
			UINT	uiCellHeight;
			UINT	uiThresholdWidth;
			UINT	uiThresholdHeight;
		};

		struct tagSvcIpcInfo
		{
			String	strHost;
			UINT	uiPort;
		};

		struct tagScreenInfo
		{
			INT		iOffsetX;
			INT		iOffsetY;
			UINT	uiWidth;
			UINT	uiHeight;
			INT		iCaptionX;
			INT		iCaptionY;
			UINT	uiCaptionW;
			UINT	uiCaptionH;
			String  strName;
		};

		struct tagTouchInfo
		{
			String	strTUIOHost;
			UINT	uiTUIOPort;
			BOOLEAN	bTUIOUse;
			String	strDeviceNode;

			UINT	uiHitOffset;
			UINT	uiWaverDistance;
			UINT	uiMoveWaverDistance;
			UINT	uiLongPressDelay;
			UINT	uiContinuePressInterval;
			UINT	uiDoubleTapInterval;
			UINT	uiDoubleTapWaverDistance;
			UINT	uiContinueTapInterval;
		};

		struct tagLanguage
		{
			String	strId;
			String	strPath;
			Map<String, String, tagStringCompare> mapName;
		};

		struct tagLanguageInfo
		{
			Array<tagLanguage> aryLanguage;
		};

		struct tagSkin
		{
			String	strId;
			Map<String, String, tagStringCompare> mapName;
			Map<String, String, tagStringCompare> mapPath;
		};

		struct tagSkinInfo
		{
			Array<tagSkin> arySkin;
		};

	private:// embed class or struct or enum

	public:// method
		static	CConfigParser*	getInstance();	
		static	void			deleteInstance();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		BOOLEAN			parse(const String &strPath);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		tagSysInfo*				getSysInfo();

		/**
		*  Inner method.
		*/
		tagTextRenderOptInfo*	getTextRenderOptInfo();

		/**
		*  Inner method.
		*/
		tagSvcIpcInfo*			getSvcIpcInfo();

		/**
		*  Inner method.
		*/
		tagScreenInfo*			getScreenInfo();

		/**
		*  Inner method.
		*/
		tagTouchInfo*			getTouchInfo();

		/**
		*  Inner method.
		*/
		tagLanguageInfo*		getLanguageInfo();

		/**
		*  Inner method.
		*/
		tagSkinInfo*			getSkinInfo();

		/**
		*  Inner method.
		*/
		const String&			getFontLibPath() const;

		void					setSkinPath(const String& strPath) { m_strSkinPath = strPath; }
		const String&			getSkinPath() const { return m_strSkinPath; }

		/**
		* return the content of [PROJECT] section.
		*/
		const Map<String, String, tagStringCompare>*	getUserSection(const String &strSection);

		void	InitScreenInfo(const tagScreenInfo& info);

	protected:// method
		CConfigParser();
		virtual ~CConfigParser();

	private:// method
		void	InitSysInfo();
		void	InitTextRenderOptInfo();
		void	InitSvcIpcInfo();
		void	InitTouchInfo();
		void	InitLanguageInfo();
		void	InitSkinInfo();

	protected:// property

	private:// property
		static	CConfigParser		*s_pInstance;

		CNodeParser				m_parser;

		tagSysInfo				m_tagSysInfo;
		tagTextRenderOptInfo	m_tagTextRenderOptInfo;
		tagSvcIpcInfo			m_tagSvcIpcInfo;
		tagScreenInfo			m_tagScreenInfo;
		tagTouchInfo			m_tagTouchInfo;
		tagLanguageInfo			m_tagLanguageInfo;
		tagSkinInfo				m_tagSkinInfo;

		String					m_strFontLibPath;
		String					m_strSkinPath;
	};
}