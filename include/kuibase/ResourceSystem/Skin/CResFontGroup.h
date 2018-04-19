/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResFontGroup : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResFontGroup();
		virtual ~CResFontGroup();

		const String&	getFontName() const;
		UINT			getFontSize() const;
		UINT			getFontColor() const;
		const String&	getFontStyle() const;

		UINT			getFontCount();
		CResFont*		getFontByIndex(UINT uiIndex);
		CResFont*		createFont();
		void			deleteFont(CResFont *pFont);
		void			removeFont(CResFont *pFont);
		void			addFont(CResFont *pFont);
		void			deleteAllFont();

		virtual void	parseProperty(const String &strData);
		virtual String	saveProperty();

	protected:// method
		CResFontGroup(const CResFontGroup &){}
		CResFontGroup &operator = (const CResFontGroup &){return *this;}	

		virtual	void	notifyPropertyChanged();

	private:// method
		void	getFontList();
		void	updateFontProperty();
		void	updateFontInfo();

	protected:// property

	private:// property
		Array2<UINT>	m_aryFont;
		CResFont		*m_pCurFont;
	};
}