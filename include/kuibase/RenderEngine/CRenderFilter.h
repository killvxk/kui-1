/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderBaseFilter : public Object , public ITimerListener
	{
	public:// embed class or struct or enum
		enum EFilterType
		{
			FT_NONE	= 0,
			FT_REFLECTION,
			FT_GLASS,
			FT_RIPPLE,
			FT_MASK,
		};

	public:// method
		virtual ~CRenderBaseFilter();

		EFilterType		getType() const		{ return m_eType;}

		void			setAssociateComponent(CComponent* pComponent)	{ m_pAssociateComponent = pComponent; }

		BOOLEAN			isPlaying()	{ return m_timer.isRunning(); }
		virtual	void	play()	{ }
		virtual	void	stop()	{ }

		BOOLEAN			isDynamic() const { return m_bDynamic;  }

		void onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		void onTimerComplete(const CTimer *pTimer);

	protected:// method
		CRenderBaseFilter();

		void			innerPlay();
		void			innerStop();

	protected:// property	
		EFilterType		m_eType;
		CComponent*		m_pAssociateComponent;
		BOOLEAN			m_bDynamic;

	private:
		CTimer m_timer;
	};

	class OPENHMI_API CRenderFltReflection : public CRenderBaseFilter
	{
	public:// method
		CRenderFltReflection();

		void			setHeight(UINT uiValue)					{ m_uiHeight = uiValue; }
		UINT			getHeight() const						{ return m_uiHeight; }

		void			setMaskImage(const String &strValue)	{ m_strMaskImage = strValue; }
		const String&	getMaskImage() const					{ return m_strMaskImage; }

	private:// property	
		UINT	m_uiHeight;
		String	m_strMaskImage;
	};

	class OPENHMI_API CRenderFltGlass : public CRenderBaseFilter
	{
	public:// method
		CRenderFltGlass();

		// Blur size - 0-100, 0 is no blur effect, it will be treated as normal glass.
		void			setBlurSize(UINT uiValue)				{ m_uiBlurSize = uiValue; }
		UINT			getBlurSize() const						{ return m_uiBlurSize; }

		void			setMaskImage(const String &strValue)	{ m_strMaskImage = strValue; }
		const String&	getMaskImage() const					{ return m_strMaskImage; }

	private:// property	
		UINT            m_uiBlurSize;
		String			m_strMaskImage;

	private:// property	

	};

	class OPENHMI_API CRenderFltRipple : public CRenderBaseFilter
	{
	public:// method
		CRenderFltRipple();

		void	setTouchRadius(UINT uiValue)			{ m_uiTouchRadius = uiValue; }
		UINT	getTouchRadius() const					{ return m_uiTouchRadius; }
		
		void	setDamping(UINT uiValue)				{ m_uiDamping = uiValue; }
		UINT    getDamping() const						{ return m_uiDamping; }

		UINT		getCursorCnt();
		CCursor		getCursorByIndex(UINT uiIndex);

		virtual	void	play()	{ innerPlay(); }
		virtual	void	stop()	{ innerStop(); }

	private:// property	
		UINT	m_uiTouchRadius;
		UINT	m_uiDamping;
	};

	class OPENHMI_API CRenderFltMask : public CRenderBaseFilter
	{
	public:// method
		CRenderFltMask();

		void			setMaskImage(const String &strValue)	{ m_strMaskImage = strValue; }
		const String&	getMaskImage() const					{ return m_strMaskImage; }

	private:// property	
		String			m_strMaskImage;

	private:// property	

	};
}