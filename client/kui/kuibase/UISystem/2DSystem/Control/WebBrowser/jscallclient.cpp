#include "OpenHMI.h"


void ygx_win_close()
{
	OpenHMI::CBaseScreen* pScreen = OpenHMI::CBaseMainFrame::getInstance()->getHomeScreen();
	pScreen->DestroyUserDlg();
}

void ygx_user_login(CString id,CString name,CString url ,CString fcm)
{
	OpenHMI::CBaseScreen* pScreen = OpenHMI::CBaseMainFrame::getInstance()->getHomeScreen();
	pScreen->UserDlgLoginSuccess(id, name, url, fcm);
}

void ygx_set_winAttr( CString title,int with,int height )
{
	OpenHMI::CBaseScreen* pScreen = OpenHMI::CBaseMainFrame::getInstance()->getHomeScreen();
	pScreen->SetUserDlgAttr(title, with, height);
}

void ygx_open_win( CString url,CString title,int with,int height )
{
	OpenHMI::CBaseScreen* pScreen = OpenHMI::CBaseMainFrame::getInstance()->getHomeScreen();
	pScreen->OpenUserDlgWin(url, title, with, height);
}

void ygx_pop_to_menu(CString url)
{
	OpenHMI::CBaseScreen* pScreen = OpenHMI::CBaseMainFrame::getInstance()->getHomeScreen();
	pScreen->PopToMenu(url);
}

void ygx_user_logout( CString id )
{
	OpenHMI::CBaseScreen* pScreen = OpenHMI::CBaseMainFrame::getInstance()->getHomeScreen();
	pScreen->UserDlgLoginOut(id);
}
