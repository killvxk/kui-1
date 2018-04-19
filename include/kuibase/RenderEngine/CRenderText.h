/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderText : public CRenderItem
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EBiDiMode
		{
			BDM_LEFT_TO_RIGHT    = 0,
			BDM_RIGHT_TO_LEFT    = 1,
		};

		enum EWrapLine
		{
			WL_NOWRAP			= 0,
			WL_WRAP_BY_CHAR		= 1,
			WL_WRAP_BY_WORD		= 2,
		};

		enum EHorizontalLayout
		{
			HL_LEFT			= 0,
			HL_CENTER		= 1,
			HL_RIGHT		= 2,
		};

		enum EVerticalLayout
		{
			VL_TOP			= 0,
			VL_MIDDLE		= 1,
			VL_BOTTOM		= 2,
		};		

	private:// embed class or struct or enum		
		friend class CRenderFont;

	public:// method		
		CRenderText();
		virtual ~CRenderText();

		void							setContent(const String &strValue);
		const String&					getContent() const								{ return m_strContent; }
		void							setBiDiMode(EBiDiMode eValue);
		CRenderText::EBiDiMode			getBiDiMode() const								{ return m_eBiDiMode; }
		void							setLineSpacing(INT iValue);
		INT								getLineSpacing() const							{ return m_iLineSpacing; }

		void							setOffsetX(INT iValue);
		INT								getOffsetX() const								{ return m_iOffsetX; }
		void							setOffsetY(INT iValue);
		INT								getOffsetY() const								{ return m_iOffsetY; }
		void							setUseOffsetX(BOOLEAN bValue);
		BOOLEAN							getUseOffsetX() const							{ return m_bUseOffsetX; }
		void							setUseOffsetY(BOOLEAN bValue);
		BOOLEAN							getUseOffsetY() const							{ return m_bUseOffsetY; }

		void							setWrapLine(EWrapLine eValue);
		CRenderText::EWrapLine			getWrapLine() const								{ return m_eWrapLine; }
		void							setHorizontalLayout(EHorizontalLayout eValue);
		CRenderText::EHorizontalLayout	getHorizontalLayout() const						{ return m_eHorizontalLayout; }
		void							setVerticalLayout(EVerticalLayout eValue);
		CRenderText::EVerticalLayout	getVerticalLayout() const						{ return m_eVerticalLayout; }

		void							setHoritontalDuplicateSwitch(BOOLEAN bValue);
		BOOLEAN							getHoritontalDuplicateSwitch() const			{ return m_bHoritontalDuplicateSwitch; }
		void							setHoritontalDuplicateInterval(INT iValue);
		INT								getHoritontalDuplicateInterval() const			{ return m_iHoritontalDuplicateInterval; }
		
		const CRenderFont*				getFont() const									{ return &m_font; }
		CRenderFont*					getFont()										{ return &m_font; }

	protected:// method
		CRenderText(const CRenderText&){}
		CRenderText& operator =(const CRenderText&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		String					m_strContent;
		EBiDiMode				m_eBiDiMode;
		INT						m_iLineSpacing;

		INT						m_iOffsetX;
		INT						m_iOffsetY;
		BOOLEAN					m_bUseOffsetX;
		BOOLEAN					m_bUseOffsetY;		

		EWrapLine				m_eWrapLine;
		EHorizontalLayout		m_eHorizontalLayout;
		EVerticalLayout			m_eVerticalLayout;

		BOOLEAN					m_bHoritontalDuplicateSwitch;
		INT						m_iHoritontalDuplicateInterval;

		CRenderFont				m_font;
	};
}