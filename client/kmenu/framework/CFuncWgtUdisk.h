#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtUdisk
		: public CBaseFuncWidget
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_TITLE								 = 1 ,
			RES_ITM_BTN_QUIT							 = 4 ,
			RES_ITM_BTN_OPEN_DISK						 = 5 ,
			RES_ITM_BTN_QUIT_DISK						 = 6 ,
			RES_ITM_TXT_USER_TITLE						 = 7 ,
			RES_ITM_TXT_DISK_SIZE_TITILE				 = 9 ,
			RES_ITM_TXT_USER							 = 15 ,
			RES_ITM_TXT_DISK_SIZE						 = 16 ,
			RES_ITM_TXT_DISK_USED_TITILE				 = 17 ,
			RES_ITM_TXT_DISK_USE_TITILE					 = 18 ,
			RES_ITM_TXT_DISK_USED						 = 19 ,
			RES_ITM_TXT_DISK_USE						 = 20 ,
			RES_ITM_BTN_CHANGE_PSW						 = 21 ,

		};

	public:// method
		CFuncWgtUdisk();
		virtual ~CFuncWgtUdisk();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnOpenDiskTchRel(CComponent *pTarget);
		void	onBtnQuitDiskTchRel(CComponent *pTarget);
		void	onBtnChangePswTchRel(CComponent *pTarget);
		void	onPopChangePsw(CBasePopup *pTarget);

		// TODO: define logic event listener here
		void	onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlTextField						m_txtTitle;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnOpenDisk;
		CCtrlButton							m_btnQuitDisk;
		CCtrlTextField						m_txtUserTitle;
		CCtrlTextField						m_txtDiskSizeTitile;
		CCtrlTextField						m_txtUser;
		CCtrlTextField						m_txtDiskSize;
		CCtrlTextField						m_txtDiskUsedTitile;
		CCtrlTextField						m_txtDiskUseTitile;
		CCtrlTextField						m_txtDiskUsed;
		CCtrlTextField						m_txtDiskUse;
		CCtrlButton							m_btnChangePsw;


		// TODO: define normal private property here
		CPopChangePsw*						m_pPopChangePsw;
	};
}
