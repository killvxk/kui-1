/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResFont : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResFont();
		virtual ~CResFont();

		void			setFontName(const String &strName);
		const String&	getFontName() const		{ return m_strFontName; }
		
		void			setFontSize(UINT uiSize);
		UINT			getFontSize() const		{ return m_uiFontSize; }
		
		void			setFontColor(UINT uiColor);
		UINT			getFontColor() const	{ return m_uiFontColor; }

		void			setFontStyle(const String &strStyle);
		const String&	getFontStyle() const	{ return m_strFontStyle; }

		CResFont*		clone();

		virtual void	parseProperty(const String &strData);
		virtual String	saveProperty();

	protected:// method
		CResFont(const CResFont &){}
		CResFont &operator = (const CResFont &){return *this;}	

		virtual	void	notifyPropertyChanged();

	private:// method
		void			updateFontInfo();

	protected:// property

	private:// property
		String	m_strFontName;
		UINT	m_uiFontSize;
		UINT	m_uiFontColor;
		String	m_strFontStyle;
	};
}