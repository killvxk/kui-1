#pragma once

namespace OpenHMI
{
	class CRenderTarget : public CacheGPUObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderTarget(UINT uiWidth,UINT uiHeight);
		virtual ~CRenderTarget();

		void	bind();
		void	unBind();
		void	clear(UINT uiOption);

		void	resize(UINT uiWidth, UINT uiHeight);

		void	setClearColor(UINT uiClr);

		UINT	getWidth() const	{ return m_uiWidth;		}
		UINT	getHeight() const	{ return m_uiHeight;	}

		void	setInUse(BOOLEAN b)	{ m_bInUse = b;			}
		BOOLEAN isInUse()	const	{ return m_bInUse;		}

		CCacheTexture*	getTexture();

	protected:// method
		CRenderTarget(const CRenderTarget &){}
		CRenderTarget &operator = (const CRenderTarget &){return *this;}	

	private:// method
		void	attachColorBuffer(CCacheTexture* pTex);
		void	unAttachColorBuffer();

	protected:// property

	private:// property	
		UINT			m_uiWidth;
		UINT			m_uiHeight;
		UINT			m_uiFBOId;
		UINT			m_uiClearColor;
		//tagResourceLoc	m_iTexID;
		CCacheTexturePtr	m_pTex;
		UINT			m_uiDepthBuffer;
		BOOLEAN			m_bInUse;
	};

	typedef ObjectPtr<CRenderTarget> CRenderTargetPtr;
}