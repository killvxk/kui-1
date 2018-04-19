#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtTips
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
			RES_ITM_TXT									 = 2 ,
			RES_ITM_BTN_QUIT							 = 5 ,
			RES_ITM_BTN_OK								 = 6 ,

		};

	public:// method
		CFuncWgtTips();
		virtual ~CFuncWgtTips();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void setContent(const String& str) { m_txt.setContent(str); }
		void setNotCatchKey() { m_bNotCatchKey = TRUE; }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnOkTchRel(CComponent *pTarget);


		// TODO: define logic event listener here
		void	onKeyEnter();
		void	onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlTextField						m_txtTitle;
		CCtrlTextField						m_txt;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnOk;


		// TODO: define normal private property here
		BOOLEAN								m_bNotCatchKey;
	};
}
