#include "RenderEngine.h"

namespace OpenHMI
{
	CRenderTarget::CRenderTarget(UINT uiWidth,UINT uiHeight)
		: m_uiWidth(uiWidth)
		, m_uiHeight(uiHeight)
		, m_uiFBOId(-1)
		, m_uiClearColor(0xffffffff)
		, m_uiDepthBuffer(0)
		, m_pTex(NULL)
		, m_bInUse(TRUE)
	{
		glGenFramebuffers(1, &m_uiFBOId);

		//glBindFramebuffer(1,m_uiFBOId);
		//glGenRenderbuffers(1, &m_uiDepthBuffer);
		//glBindRenderbuffer(GL_RENDERBUFFER, m_uiDepthBuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, uiWidth, uiHeight);

		
		CCacheTexture* pTex = CTexturePool::getInstance()->getTexture(this);
		bind();
		attachColorBuffer(pTex);
		unBind();
	}

	CRenderTarget::~CRenderTarget()
	{
		bind();
		unAttachColorBuffer();
		unBind();
		glDeleteFramebuffers(1, &m_uiFBOId);
	}

	void CRenderTarget::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_uiFBOId);
	}

	void CRenderTarget::unBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CRenderTarget::clear(UINT uiOption)
	{
		glClearColor(0.0f,0.0f,0.0f,0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0,0,m_uiWidth,m_uiHeight);
	}

	void CRenderTarget::resize(UINT uiWidth, UINT uiHeight)
	{
		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;

		CCacheTexture* pTex = CTexturePool::getInstance()->getTexture(this);
		bind();
		unAttachColorBuffer();
		attachColorBuffer(pTex);
		unBind();
	}

	void CRenderTarget::setClearColor(UINT uiClr)
	{
		m_uiClearColor = uiClr;
	}

	CCacheTexture*	CRenderTarget::getTexture()
	{
		return m_pTex;
	}
	
	void CRenderTarget::attachColorBuffer(CCacheTexture* pTex)
	{
		m_pTex = pTex;

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTex->getTexID(), 0);
	}

	void CRenderTarget::unAttachColorBuffer()
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		
		m_pTex = NULL;
	}
}