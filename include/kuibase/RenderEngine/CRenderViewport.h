/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderViewport : public CRenderItem
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EStyleType
		{
			ST_NORMAL	= 0,
			ST_TRANSPARENT,
			ST_CAPTURE,
			ST_WAVER,
		};

		class OPENHMI_API CBaseStyle : public Object
		{
		public:// method
			virtual ~CBaseStyle(){}

			EStyleType		getType() const		{ return m_eType;}

		protected:// method
			CBaseStyle();

		protected:// property	
			EStyleType		m_eType;
		};

		class OPENHMI_API CStyleNormal : public CBaseStyle
		{
		public:// method
			CStyleNormal();

		};

		class OPENHMI_API CStyleTransparent : public CBaseStyle
		{
		public:// method
			CStyleTransparent();

		};

		class OPENHMI_API CStyleCapture : public CBaseStyle
		{
		public:// method
			CStyleCapture();

		};

		class OPENHMI_API CStyleWaver : public CBaseStyle
		{
		public:// method
			CStyleWaver();

			UINT	uiWidth;
			UINT	uiHeight;
			UINT	uiRow;
			UINT	uiColumn;
			String	strTextureImage;			
		};		

	private:// embed class or struct or enum		

	public:// method		
		CRenderViewport(BOOLEAN bManageLifeCycle = FALSE);
		virtual ~CRenderViewport();

		void								setStyle(CBaseStyle *pStyle);
		CRenderViewport::CBaseStyle*		getStyle() { return m_pStyle; }
		void								updateStyle();

	protected:// method
		CRenderViewport(const CRenderViewport&){}
		CRenderViewport& operator =(const CRenderViewport&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		BOOLEAN			m_bManageLifeCycle;
		CBaseStyle*		m_pStyle;		
	};
}