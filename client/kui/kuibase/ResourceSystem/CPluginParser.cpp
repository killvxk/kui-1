#include "OpenHMI.h"

namespace OpenHMI
{
	CPluginParser	*CPluginParser::s_pInstance	= NULL;

	CPluginParser* CPluginParser::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CPluginParser();
		}

		return s_pInstance;
	}

	void CPluginParser::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CPluginParser::CPluginParser()
	{
		
	}

	CPluginParser::~CPluginParser()
	{
		;
	}

	BOOLEAN CPluginParser::parse(const String &strPath)
	{
		CNodeParser	parser;

		if (parser.parse(strPath) == FALSE)
		{
			return FALSE;
		}

		m_strRenderEngineWin			= parser.getItemString(L"PLUGIN", L"render_engine_win");
		m_strRenderEngineQnx			= parser.getItemString(L"PLUGIN", L"render_engine_qnx");
		m_strRenderEngineLinux			= parser.getItemString(L"PLUGIN", L"render_engine_linux");
		m_strServiceAdapterWin			= parser.getItemString(L"PLUGIN", L"service_adapter_win");
		m_strServiceAdapterQnx			= parser.getItemString(L"PLUGIN", L"service_adapter_qnx");
		m_strServiceAdapteLinux			= parser.getItemString(L"PLUGIN", L"service_adapter_linux");
		m_strGestureRecognizerWin		= parser.getItemString(L"PLUGIN", L"gesture_recognizer_win");
		m_strGestureRecognizerQnx		= parser.getItemString(L"PLUGIN", L"gesture_recognizer_qnx");
		m_strGestureRecognizerLinux		= parser.getItemString(L"PLUGIN", L"gesture_recognizer_linux");

		return TRUE;
	}

	const String& CPluginParser::getRenderEnginePlugin()
	{
		m_strRenderEngineQnx = L"./kopengles.dll";
		return m_strRenderEngineQnx;
	}

	const String& CPluginParser::getServiceAdapterPlugin()
	{
#ifdef OS_WIN
		return m_strServiceAdapterWin;
#endif

#ifdef OS_QNX
		return m_strServiceAdapterQnx;
#endif

#ifdef OS_LINUX
		return m_strServiceAdapteLinux;
#endif
	}

	const String& CPluginParser::getGestureRecognizerPlugin()
	{
#ifdef OS_WIN
		return m_strGestureRecognizerWin;
#endif

#ifdef OS_QNX
		return m_strGestureRecognizerQnx;
#endif

#ifdef OS_LINUX
		return m_strGestureRecognizerLinux;
#endif
	}
}