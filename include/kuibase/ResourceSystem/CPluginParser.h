/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CPluginParser
	{
	public:// const define

	private:// const define
		
	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	CPluginParser*	getInstance();	
		static	void			deleteInstance();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		BOOLEAN			parse(const String &strPath);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		const String&	getRenderEnginePlugin();
		const String&	getServiceAdapterPlugin();
		const String&	getGestureRecognizerPlugin();


	protected:// method
		CPluginParser();
		virtual ~CPluginParser();

	private:// method

	protected:// property

	private:// property
		static	CPluginParser		*s_pInstance;

		String			m_strRenderEngineWin;
		String			m_strRenderEngineQnx;
		String			m_strRenderEngineLinux;
		String			m_strServiceAdapterWin;
		String			m_strServiceAdapterQnx;
		String			m_strServiceAdapteLinux;
		String			m_strGestureRecognizerWin;
		String			m_strGestureRecognizerQnx;
		String			m_strGestureRecognizerLinux;
	};
}