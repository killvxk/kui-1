#include "RenderEngine.h"
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus; 

namespace OpenHMI
{
	CRenderEngineOS	*CRenderEngineOS::s_pInstance	= NULL;

	CRenderEngineOS* CRenderEngineOS::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CRenderEngineOS();
		}

		return s_pInstance;
	}

	void CRenderEngineOS::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	tagRenderContext::tagRenderContext() 
	{
		reset();
	}

	void tagRenderContext::reset()
	{
#ifdef OS_WIN

#endif

#ifdef OS_QNX
	#ifdef CHIPSET_JACINTO4
		pBuf = NULL;
		pbtBufData = NULL;
	#endif

	#ifdef CHIPSET_JACINTO5
		pBuf = NULL;
		pbtBufData = NULL;
	#endif

	#ifdef CHIPSET_IMX6SOLO
		pBuf = NULL;
		pbtBufData = NULL;
	#endif
#endif

#ifdef OS_LINUX
	#ifdef CHIPSET_JACINTO4
		iFbfd = 0;
		iScreenSize = 0;	
		pbtFB = NULL;
	#endif

	#ifdef CHIPSET_IMX6SOLO
		iFbfd = 0;
		iScreenSize = 0;	
		pbtFB = NULL;
	#endif

	#ifdef CHIPSET_NBTEVO
		pImg = NULL;
		pbtBufData = NULL;
	#endif

	#ifdef CHIPSET_PCIBM
		pImg = NULL;
		pbtBufData = NULL;
	#endif
#endif
	}

	CRenderEngineOS::CRenderEngineOS()
		: m_bInitialized(FALSE)
	{

	}

	CRenderEngineOS::~CRenderEngineOS()
	{
		;
	}

	BOOLEAN CRenderEngineOS::initialize(const tagScreenContext *pScreenContext)
	{
		if (m_bInitialized)
		{
			return m_bInitialized;
		}		
		// Initialize GDI+.
		GdiplusStartupInput gdiplusStartupInput;

		GdiplusStartup(&m_gdiplusToken,&gdiplusStartupInput,NULL);

		// add initialize code here
		if (pScreenContext != NULL)
		{
			m_bInitialized = TRUE; 
			m_screenContext = *pScreenContext;
			m_bInitialized = initializeSession(pScreenContext, &m_renderContext);
			return m_bInitialized;
		}

		m_bInitialized = TRUE;
		return m_bInitialized;
	}

	void CRenderEngineOS::unInitialize(const tagScreenContext *pScreenContext)
	{
		if (!m_bInitialized)
		{
			return;
		}
		m_bInitialized = FALSE;

		// add un-initialize code here
		if (pScreenContext != NULL)
		{			
			unInitializeSession(pScreenContext, &m_renderContext);
			m_screenContext.reset();
		}
		if(m_gdiplusToken != NULL)
		{
			GdiplusShutdown(m_gdiplusToken);
		}
	}

#ifdef OS_WIN
	BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
	{
		if (!m_bInitialized)
		{
			return FALSE;
		}

		if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
		{
			return FALSE;
		}

		// add initialize code here
		if (pScreenContext->hWnd == NULL || pScreenContext->hDC == NULL)
		{
			return FALSE;
		}

		::SetStretchBltMode(pScreenContext->hDC, COLORONCOLOR);

		return TRUE;
	}

	void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
	{
		if (!m_bInitialized)
		{
			return;
		}

		// add un-initialize code here
		pRenderContext->reset();
	}
#endif

#ifdef OS_QNX
	#ifdef CHIPSET_JACINTO4
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			INT iVal = SCREEN_USAGE_WRITE;
			if (screen_set_window_property_iv( pScreenContext->screen_win, SCREEN_PROPERTY_USAGE, &iVal ))
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen set usage property error.");
				return FALSE;
			}
			
			if (screen_create_window_buffers( pScreenContext->screen_win, 1 )) 
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen create window buffer error.");
				return FALSE;
			}
		
			if (screen_get_window_property_pv( pScreenContext->screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&(pRenderContext->pBuf ) ) )
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen get render buffer property error.");
				return FALSE;
			}

			if (screen_get_buffer_property_pv( pRenderContext->pBuf, SCREEN_PROPERTY_POINTER, (void **)&(pRenderContext->pbtBufData) ) < 0)
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen get pointer property error.");
				return FALSE;
			}

			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pScreenContext->screen_win != NULL)
			{
				screen_destroy_window_buffers(pScreenContext->screen_win);
			}

			pRenderContext->reset();
		}
	#endif

	#ifdef CHIPSET_JACINTO5
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			INT iVal = SCREEN_USAGE_WRITE;
			if (screen_set_window_property_iv( pScreenContext->screen_win, SCREEN_PROPERTY_USAGE, &iVal ))
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen set usage property error.");
				return FALSE;
			}

			if (screen_create_window_buffers( pScreenContext->screen_win, 1 )) 
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen create window buffer error.");
				return FALSE;
			}

			if (screen_get_window_property_pv( pScreenContext->screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&(pRenderContext->pBuf ) ) )
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen get render buffer property error.");
				return FALSE;
			}

			if (screen_get_buffer_property_pv( pRenderContext->pBuf, SCREEN_PROPERTY_POINTER, (void **)&(pRenderContext->pbtBufData) ) < 0)
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen get pointer property error.");
				return FALSE;
			}

			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pScreenContext->screen_win != NULL)
			{
				screen_destroy_window_buffers(pScreenContext->screen_win);
			}

			pRenderContext->reset();
		}
	#endif

	#ifdef CHIPSET_IMX6SOLO
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			INT iVal = SCREEN_USAGE_WRITE;
			if (screen_set_window_property_iv( pScreenContext->screen_win, SCREEN_PROPERTY_USAGE, &iVal ))
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen set usage property error.");
				return FALSE;
			}

			if (screen_create_window_buffers( pScreenContext->screen_win, 1 )) 
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen create window buffer error.");
				return FALSE;
			}

			if (screen_get_window_property_pv( pScreenContext->screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&(pRenderContext->pBuf ) ) )
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen get render buffer property error.");
				return FALSE;
			}

			if (screen_get_buffer_property_pv( pRenderContext->pBuf, SCREEN_PROPERTY_POINTER, (void **)&(pRenderContext->pbtBufData) ) < 0)
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Screen get pointer property error.");
				return FALSE;
			}

			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pScreenContext->screen_win != NULL)
			{
				screen_destroy_window_buffers(pScreenContext->screen_win);
			}

			pRenderContext->reset();
		}
	#endif
#endif

#ifdef OS_LINUX
	#ifdef CHIPSET_JACINTO4
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			// TODO : read from config, may multi dev
			const char* pcDevFile = "/dev/fb0";

			pRenderContext->iFbfd = open(pcDevFile, O_RDWR);  
			if (!pRenderContext->iFbfd)  
			{  
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer open device error.");
				return FALSE;
			}

			// for TI
			struct ti81xxfb_region_params regp; 
			if (ioctl(pRenderContext->iFbfd, TIFB_GET_PARAMS,  &regp) != 0) 	
			{ 
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer get param error.");
				return FALSE;
			} 
			// Set Global Alpha Blending
			regp.blendtype = TI81XXFB_BLENDING_PIXEL; // TI81XXFB_BLENDING_GLOBAL;
			regp.blendalpha = 128; //alp2; 
			if (ioctl(pRenderContext->iFbfd, TIFB_SET_PARAMS, &regp) != 0) 
			{ 
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer set param error.");
				return FALSE;
			}

			fb_var_screeninfo vinfo;
			if (ioctl(pRenderContext->iFbfd, FBIOGET_VSCREENINFO, &vinfo))  
			{ 
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer read variable information error.");
				return FALSE;
			} 

			if (vinfo.xres != pScreenContext->uiScreenWidth || vinfo.yres != pScreenContext->uiScreenHeight || vinfo.bits_per_pixel != 32)
			{
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer information error.");
				return FALSE;
			}

			pRenderContext->iScreenSize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;  

			pRenderContext->pbtFB = (BYTE*)mmap(0, pRenderContext->iScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, pRenderContext->iFbfd, 0);  
			if ((void*)pRenderContext->pbtFB == MAP_FAILED)  // MAP_FAILED = (void*)-1
			{  
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer map device to memory error.");
				pRenderContext->pbtFB = NULL;
				return FALSE;
			}

			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pRenderContext->pbtFB != NULL)
			{
				munmap(pRenderContext->pbtFB, pRenderContext->iScreenSize);
			}

			if (pRenderContext->iFbfd > 0)
			{
				close(pRenderContext->iFbfd);
			}

			pRenderContext->reset();
		}
	#endif

	#ifdef CHIPSET_IMX6SOLO
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			// TODO : read from config, may multi dev
			const char* pcDevFile = "/dev/fb0";

			pRenderContext->iFbfd = open(pcDevFile, O_RDWR);  
			if (!pRenderContext->iFbfd)  
			{  
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer open device error.");
				return FALSE;
			}

			// for TI
// 			struct ti81xxfb_region_params regp; 
// 			if (ioctl(pRenderContext->iFbfd, TIFB_GET_PARAMS,  &regp) != 0) 	
//			{ 
//				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer get param error.");
// 				return FALSE;
// 			} 
//			// Set Global Alpha Blending
// 			regp.blendtype = TI81XXFB_BLENDING_PIXEL; // TI81XXFB_BLENDING_GLOBAL;
// 			regp.blendalpha = alp2; 
// 			if (ioctl(pRenderContext->iFbfd, TIFB_SET_PARAMS, &regp) != 0) 
//			{ 
//				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer set param error.");
// 				return FALSE;
// 			}

			fb_var_screeninfo vinfo;
			if (ioctl(pRenderContext->iFbfd, FBIOGET_VSCREENINFO, &vinfo))  
			{ 
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer read variable information error.");
				return FALSE;
			}
			
			CLog::info(String(RENDER_ENGINE_NAME), String::format(L"vinfo.xres := %d,  vinfo.yres := %d,  vinfo.bits_per_pixel := %d", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel));
			
			//vinfo.bits_per_pixel = 32;
			//vinfo.yoffset = 0;
			//ioctl(pRenderContext->iFbfd, FBIOPUT_VSCREENINFO, &vinfo);
			//if (ioctl(pRenderContext->iFbfd, FBIOPUT_VSCREENINFO, &vinfo) < 0);
			//{
				//CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer set screen_info error.");
				//return FALSE;
			//}

			if (vinfo.xres != pScreenContext->uiScreenWidth || vinfo.yres != pScreenContext->uiScreenHeight || vinfo.bits_per_pixel != 32)
			{
				CLog::info(String(RENDER_ENGINE_NAME), String::format(L"vinfo.xres := %d,  vinfo.yres := %d,  vinfo.bits_per_pixel := %d", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel));
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer information error.");
				return FALSE;
			}

			pRenderContext->iScreenSize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;  

			pRenderContext->pbtFB = (BYTE*)mmap(0, pRenderContext->iScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, pRenderContext->iFbfd, 0);  
			if ((void*)pRenderContext->pbtFB == MAP_FAILED)  // MAP_FAILED = (void*)-1
			{  
				CLog::error2(RENDER_ENGINE_NAME, L"Frame buffer map device to memory error.");
				pRenderContext->pbtFB = NULL;
				return FALSE;
			}
			
			UINT i;
			for (i = 0; i < pRenderContext->iScreenSize/4; i++)
			{
                ((UINT*)pRenderContext->pbtFB)[i] = 0x00FF0000;
			}	
			sleep(3);


			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pRenderContext->pbtFB != NULL)
			{
				munmap(pRenderContext->pbtFB, pRenderContext->iScreenSize);
			}

			if (pRenderContext->iFbfd > 0)
			{
				close(pRenderContext->iFbfd);
			}

			pRenderContext->reset();
		}
	#endif

	#ifdef CHIPSET_NBTEVO
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			pRenderContext->pbtBufData = new BYTE[pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4];

			INT iDepth = DefaultDepth(pScreenContext->pDisplay, DefaultScreen(pScreenContext->pDisplay));
			pRenderContext->pImg = XCreateImage(pScreenContext->pDisplay, 
												CopyFromParent, 
												iDepth, 
												ZPixmap, 0, 
												(char*)pRenderContext->pbtBufData,
												pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight,
												32, 0);
			XInitImage(pRenderContext->pImg);

			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pRenderContext->pbtBufData != NULL)
			{
				delete [] pRenderContext->pbtBufData;
			}

			pRenderContext->reset();
		}
	#endif

	#ifdef CHIPSET_PCIBM
		BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return FALSE;
			}

			if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
			{
				return FALSE;
			}

			// add initialize code here
			pRenderContext->pbtBufData = new BYTE[pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4];

			INT iDepth = DefaultDepth(pScreenContext->pDisplay, DefaultScreen(pScreenContext->pDisplay));
			pRenderContext->pImg = XCreateImage(pScreenContext->pDisplay, 
												CopyFromParent, 
												iDepth, 
												ZPixmap, 0, 
												(char*)pRenderContext->pbtBufData,
												pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight,
												32, 0);
			XInitImage(pRenderContext->pImg);

			return TRUE;
		}

		void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
		{
			if (!m_bInitialized)
			{
				return;
			}

			// add un-initialize code here
			if (pRenderContext->pbtBufData != NULL)
			{
				delete [] pRenderContext->pbtBufData;
			}

			pRenderContext->reset();
		}
	#endif

#endif
}