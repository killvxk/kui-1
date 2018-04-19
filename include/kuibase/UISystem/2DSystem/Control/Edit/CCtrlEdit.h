/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventEdit
	{
	public:
		static const UINT TOUCH_RELEASED			= 1;
	};

	class OPENHMI_API CCtrlEdit : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlEdit();
		virtual ~CCtrlEdit();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	void	refresh();
		virtual	void	onTouchPress(INT iX, INT iY);

		void	show(BOOLEAN bValue);
		void	setEditID(UINT uiID);
		void	setFont(INT yHeight, COLORREF color, const String& strName);
		void	setFontBkClr(COLORREF color);
		void    setBkColor(COLORREF color);
		void	setContent(const CHAR* str);
		void	setLimitText(INT num);
		const String&	getContent();
		void	setFocus(BOOLEAN bValue);
		BOOLEAN	getFocus();

	protected:// method
		CCtrlEdit(const CCtrlEdit&){}
		CCtrlEdit& operator =(const CCtrlEdit&){return *this;}

	private:// method

	protected:// property

	private:// property
		HFONT m_hFont;
		const tagEditContext* m_pContext;
		String	m_strContent;
		BOOLEAN m_bSetFocus;
		BOOLEAN m_bCanFocus;
	};
}