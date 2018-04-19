/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderFont : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		// use bit calculation
		enum EFontStyle
		{
			FS_REGULAR		= 0x00,
			FS_BOLD			= 0x01,
			FS_ITALIC		= 0x02,
			FS_BOLDITALIC	= 0x03,
		};

	private:// embed class or struct or enum		

	public:// method		
		CRenderFont();
		virtual ~CRenderFont();

		void			setParentText(CRenderText *pText)	{ m_pText = pText; }

		void			setName(const String &strValue);
		const String&	getName() const						{ return m_strName; }
		void			setColor(UINT uiValue);
		UINT			getColor() const					{ return m_uiColor; }
		void			setSize(UINT uiValue);
		UINT			getSize() const						{ return m_uiSize; }
		void			setStyle(EFontStyle eValue);
		EFontStyle		getStyle() const					{ return m_eStyle; }

	protected:// method
		CRenderFont(const CRenderFont&){}
		CRenderFont& operator =(const CRenderFont&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		CRenderText		*m_pText;

		String			m_strName;
		UINT			m_uiColor;
		UINT			m_uiSize;
		EFontStyle		m_eStyle;
	};
}