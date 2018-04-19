#include "OpenHMI.h"

namespace OpenHMI
{
	CConfigParser	*CConfigParser::s_pInstance	= NULL;

	CConfigParser* CConfigParser::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CConfigParser();
		}

		return s_pInstance;
	}

	void CConfigParser::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CConfigParser::CConfigParser()
	{
		m_tagSysInfo.bUseTempCode = FALSE;
		m_tagSysInfo.bNeedTray = FALSE;

		m_tagSysInfo.bOutputPerformance = FALSE;
		m_tagSysInfo.bOutputErrorLog = TRUE;
		m_tagSysInfo.bOutputWarningLog = TRUE;
		m_tagSysInfo.bOutputInfoLog = TRUE;
		m_tagSysInfo.bOutputDebugLog = FALSE;

		m_tagSysInfo.uiCharacterCacheCount = 100000;
		m_tagSysInfo.uiImageCacheCount = 100000;
		m_tagSysInfo.uiTextureCacheCount = 100000;
		m_tagSysInfo.uiProgramCacheCount = 100000;
		m_tagSysInfo.uiShaderCacheCount = 100000;
		m_tagSysInfo.uiMaterialCacheCount = 100000;
		m_tagSysInfo.uiMeshCacheCount = 100000;
		m_tagSysInfo.uiWebBrowserCount = 0;
		m_tagSysInfo.uiEditCtrlCount = 0;
		
		m_tagSysInfo.iMsgQueueThreadPri = 51;
		m_tagSysInfo.iScreenTouchThreadPri = 51;
		m_tagSysInfo.iTimerThreadPri = 51;
		m_tagSysInfo.iServiceAdapterThreadPri = 51;
		m_tagSysInfo.iTuioWrapperThreadPri = 51;

		m_tagTextRenderOptInfo.uiWidth = 2048;
		m_tagTextRenderOptInfo.uiHeight = 2048;
		m_tagTextRenderOptInfo.uiCellWidth = 64;
		m_tagTextRenderOptInfo.uiCellHeight = 64;
		m_tagTextRenderOptInfo.uiThresholdWidth = 2048;
		m_tagTextRenderOptInfo.uiThresholdHeight = 2048;


		m_tagSvcIpcInfo.strHost = L"127.0.0.1";
		m_tagSvcIpcInfo.uiPort = 843;

		m_tagScreenInfo.iOffsetX = 0;
		m_tagScreenInfo.iOffsetY = 0;
		m_tagScreenInfo.uiWidth = 0;
		m_tagScreenInfo.uiHeight = 0;
		m_tagScreenInfo.strName = L"KUI";

		m_tagTouchInfo.strTUIOHost = L"127.0.0.1";
		m_tagTouchInfo.uiTUIOPort = 3334;
		m_tagTouchInfo.bTUIOUse = FALSE;
		m_tagTouchInfo.uiHitOffset = 0;
		m_tagTouchInfo.uiWaverDistance = 25;
		m_tagTouchInfo.uiMoveWaverDistance = 0;
		m_tagTouchInfo.uiLongPressDelay = 1000;
		m_tagTouchInfo.uiContinuePressInterval = 100;
		m_tagTouchInfo.uiDoubleTapInterval = 300;
		m_tagTouchInfo.uiDoubleTapWaverDistance = 15;
		m_tagTouchInfo.uiContinueTapInterval = 1000;
	}

	CConfigParser::~CConfigParser()
	{
		;
	}

	BOOLEAN CConfigParser::parse(const String &strPath)
	{
		if (m_parser.parse(m_strSkinPath + strPath) == FALSE)
		{
			return FALSE;
		}

		InitSysInfo();
		//InitTextRenderOptInfo();
		//InitSvcIpcInfo();
		//InitScreenInfo();
		//InitTouchInfo();
		InitLanguageInfo();
		InitSkinInfo();

		m_strFontLibPath = L"res/Font/Font.ini";
		m_parser.deleteSection(L"FONTLIB");

		return TRUE;
	}

	CConfigParser::tagSysInfo* CConfigParser::getSysInfo()
	{
		return &m_tagSysInfo;
	}

	CConfigParser::tagTextRenderOptInfo* CConfigParser::getTextRenderOptInfo()
	{
		return &m_tagTextRenderOptInfo;
	}

	CConfigParser::tagSvcIpcInfo* CConfigParser::getSvcIpcInfo()
	{
		return &m_tagSvcIpcInfo;
	}

	CConfigParser::tagScreenInfo* CConfigParser::getScreenInfo()
	{
		return &m_tagScreenInfo;
	}

	CConfigParser::tagTouchInfo* CConfigParser::getTouchInfo()
	{
		return &m_tagTouchInfo;
	}

	CConfigParser::tagLanguageInfo* CConfigParser::getLanguageInfo()
	{
		return &m_tagLanguageInfo;
	}

	CConfigParser::tagSkinInfo* CConfigParser::getSkinInfo()
	{
		return &m_tagSkinInfo;
	}

	const String& CConfigParser::getFontLibPath() const
	{
		return m_strFontLibPath;
	}

	const Map<String, String, tagStringCompare>* CConfigParser::getUserSection(const String &strSection)
	{
		return m_parser.getSection(strSection);
	}

	void CConfigParser::InitSysInfo()
	{
		m_tagSysInfo.bNeedTray				= m_parser.getItemBool(L"SYS_INFO", L"need_tray");
		//m_tagSysInfo.uiCharacterCacheCount	= m_parser.getItemUInt(L"SYS_INFO", L"character_cache_count");
		//m_tagSysInfo.uiImageCacheCount		= m_parser.getItemUInt(L"SYS_INFO", L"image_cache_count");		
		//m_tagSysInfo.uiTextureCacheCount	= m_parser.getItemUInt(L"SYS_INFO", L"texture_cache_count");
		//m_tagSysInfo.uiProgramCacheCount	= m_parser.getItemUInt(L"SYS_INFO", L"program_cache_count");
		//m_tagSysInfo.uiShaderCacheCount		= m_parser.getItemUInt(L"SYS_INFO", L"shader_cache_count");
		//m_tagSysInfo.uiMaterialCacheCount	= m_parser.getItemUInt(L"SYS_INFO", L"material_cache_count");
		//m_tagSysInfo.uiMeshCacheCount		= m_parser.getItemUInt(L"SYS_INFO", L"mesh_cache_count");
		m_tagSysInfo.uiWebBrowserCount		= m_parser.getItemUInt(L"SYS_INFO", L"web_browser_count");	
		m_tagSysInfo.uiEditCtrlCount		= m_parser.getItemUInt(L"SYS_INFO", L"edit_ctrl_count");	

		m_parser.deleteSection(L"SYS_INFO");
	}

	void CConfigParser::InitTextRenderOptInfo()
	{
		m_tagSvcIpcInfo.strHost = m_parser.getItemString(L"SVCIPC_GATEWAY", L"host");
		m_tagSvcIpcInfo.uiPort = m_parser.getItemUInt(L"SVCIPC_GATEWAY", L"port");

		m_parser.deleteSection(L"SVCIPC_GATEWAY");
	}

	void CConfigParser::InitSvcIpcInfo()
	{
		m_tagTextRenderOptInfo.uiWidth			= m_parser.getItemUInt(L"TEXTRENDER_OPTIMIZE", L"texture_width");
		m_tagTextRenderOptInfo.uiHeight			= m_parser.getItemUInt(L"TEXTRENDER_OPTIMIZE", L"texture_height");
		m_tagTextRenderOptInfo.uiCellWidth		= m_parser.getItemUInt(L"TEXTRENDER_OPTIMIZE", L"texture_cellX");
		m_tagTextRenderOptInfo.uiCellHeight		= m_parser.getItemUInt(L"TEXTRENDER_OPTIMIZE", L"texture_cellY");
		m_tagTextRenderOptInfo.uiThresholdWidth = m_parser.getItemUInt(L"TEXTRENDER_OPTIMIZE", L"texture_threshold_width");
		m_tagTextRenderOptInfo.uiThresholdHeight = m_parser.getItemUInt(L"TEXTRENDER_OPTIMIZE", L"texture_threshold_height");

		m_parser.deleteSection(L"TEXTRENDER_OPTIMIZE");
	}

	void CConfigParser::InitScreenInfo(const tagScreenInfo& info)
	{
		m_tagScreenInfo.iOffsetX	= 0;
		m_tagScreenInfo.iOffsetY	= 0;
		m_tagScreenInfo.uiWidth		= info.uiWidth;
		m_tagScreenInfo.uiHeight	= info.uiHeight;
		m_tagScreenInfo.iCaptionX	= 0;
		m_tagScreenInfo.iCaptionY	= 0;
		m_tagScreenInfo.uiCaptionW	= info.uiCaptionW;
		m_tagScreenInfo.uiCaptionH	= info.uiCaptionH;
		m_tagScreenInfo.strName		= info.strName;

		m_parser.deleteSection(L"SCREEN_INFO");
	}

	void CConfigParser::InitTouchInfo()
	{
		m_tagTouchInfo.strTUIOHost				= m_parser.getItemString(L"TOUCH_INFO", L"TUIO_host");
		m_tagTouchInfo.uiTUIOPort				= m_parser.getItemUInt(L"TOUCH_INFO", L"TUIO_port");
		m_tagTouchInfo.bTUIOUse					= m_parser.getItemBool(L"TOUCH_INFO", L"TUIO_use");
		m_tagTouchInfo.strDeviceNode			= m_parser.getItemString(L"TOUCH_INFO", L"device_node");
		
		m_tagTouchInfo.uiHitOffset				= m_parser.getItemInt(L"TOUCH_INFO", L"hit_offset");
		m_tagTouchInfo.uiWaverDistance			= m_parser.getItemInt(L"TOUCH_INFO", L"waver_distance");
		m_tagTouchInfo.uiMoveWaverDistance		= m_parser.getItemInt(L"TOUCH_INFO", L"move_waver_distance");
		m_tagTouchInfo.uiLongPressDelay			= m_parser.getItemInt(L"TOUCH_INFO", L"long_press_delay");
		m_tagTouchInfo.uiContinuePressInterval	= m_parser.getItemInt(L"TOUCH_INFO", L"continue_press_interval");
		m_tagTouchInfo.uiDoubleTapInterval		= m_parser.getItemInt(L"TOUCH_INFO", L"double_tap_interval");
		m_tagTouchInfo.uiDoubleTapWaverDistance = m_parser.getItemInt(L"TOUCH_INFO", L"double_tap_waver_distance");
		m_tagTouchInfo.uiContinueTapInterval	= m_parser.getItemInt(L"TOUCH_INFO", L"continue_tap_interval");
		
		m_parser.deleteSection(L"TOUCH_INFO");
	}

	void CConfigParser::InitLanguageInfo()
	{
		Map<String,String,tagStringCompare> *pmapLan = m_parser.getSection(L"LANGUAGE");

		if (pmapLan == NULL)
		{
			return ;
		}

		INT iSize = pmapLan->size();

		for (INT iLanIndex = 0; iLanIndex < iSize; iLanIndex++)
		{
			INT iNameIndex = 0;

			// get id
			String strId = String::format(L"lag_%d_id", iLanIndex + 1);
			String *pstrId = pmapLan->getValueByKey(strId);
			

			if (pstrId != NULL && *pstrId != STRING_NULL)
			{
				tagLanguage *pLanguage = new tagLanguage;
				m_tagLanguageInfo.aryLanguage.addItem(pLanguage);
				pLanguage->strId = *pstrId;

				String strPath = String::format(L"lag_%d_path", iLanIndex + 1);
				String *pstrPath = pmapLan->getValueByKey(strPath);
				pLanguage->strPath = *pstrPath;

				while (++iNameIndex)
				{
					// get name
					String strName = String::format(L"lag_%d_name_%d", iLanIndex + 1, iNameIndex);
					String *pstrName = pmapLan->getValueByKey(strName);

					if (pstrName != NULL && *pstrName != STRING_NULL)
					{
						INT iIndex = pstrName->indexOf(L":");
						String strTag = pstrName->subString(0, iIndex);

						INT iSize = pstrName->length() - iIndex + 1;
						ArrayPtr<CHAR> pcValue(new CHAR[iSize], iSize);
						String::strcpy(pcValue, pstrName->subString(iIndex + 1));
						String *pstrValue = new String(pcValue);

						pLanguage->mapName.addItem(strTag, pstrValue);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}

		m_parser.deleteSection(L"LANGUAGE");
	}

	void CConfigParser::InitSkinInfo()
	{
		Map<String,String,tagStringCompare> *pmapSkin = m_parser.getSection(L"SKIN");

		if (pmapSkin == NULL)
		{
			return ;
		}

		INT iSize = pmapSkin->size();

		for (INT iSkinIndex = 0; iSkinIndex < iSize; iSkinIndex++)
		{
			// get id
			String strId = String::format(L"skin_%d_id", iSkinIndex + 1 );
			String *pstrId = pmapSkin->getValueByKey(strId);

			if (pstrId != NULL && *pstrId != STRING_NULL)
			{
				INT iNameIndex = 0;
				INT iPathIndex = 0;
				tagSkin *pSkin = new tagSkin;
				pSkin->strId = *pstrId;

				m_tagSkinInfo.arySkin.addItem(pSkin);

				while (++iNameIndex) 
				{
					// get name
					String strName = String::format(L"skin_%d_name_%d", iSkinIndex + 1, iNameIndex);
					String *pstrName = pmapSkin->getValueByKey(strName);

					if (pstrName != NULL && *pstrName != STRING_NULL)
					{
						INT iIndex = pstrName->indexOf(L":");
						String strTag = pstrName->subString(0, iIndex);
						
						INT iSize = pstrName->length() - iIndex + 1;
						ArrayPtr<CHAR> pcValue(new CHAR[iSize], iSize);
						String::strcpy(pcValue, pstrName->subString(iIndex + 1));
						String *pstrValue = new String(pcValue);

						pSkin->mapName.addItem(strTag, pstrValue);
					}
					else
					{
						break;
					}
				}

				while (++iPathIndex) 
				{
					// get path
					String strPath = String::format(L"skin_%d_path_%d", iSkinIndex + 1, iPathIndex);
					String *pstrPath = pmapSkin->getValueByKey(strPath);

					if (pstrPath != NULL && *pstrPath != STRING_NULL)
					{
						INT iIndex = pstrPath->indexOf(L":");
						String strTag = pstrPath->subString(0, iIndex);

						INT iSize = pstrPath->length() - iIndex + 1;
						ArrayPtr<CHAR> pcValue(new CHAR[iSize], iSize);
						String::strcpy(pcValue, pstrPath->subString(iIndex + 1));
						String *pstrValue = new String(pcValue);

						pSkin->mapPath.addItem(strTag, pstrValue);
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				break;
			}
		}

		m_parser.deleteSection(L"SKIN");
	}
}