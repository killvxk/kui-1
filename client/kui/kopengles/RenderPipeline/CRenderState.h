#pragma once

namespace OpenHMI
{
	class CRenderState
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EBlendFactor
		{
			EBF_ONE						= GL_ONE,
			EBF_ZERO					= GL_ZERO,
			EBF_SRC_ALPHA				= GL_SRC_ALPHA,
			EBF_ONE_MINUS_SRC_ALPHA		= GL_ONE_MINUS_SRC_ALPHA,
			EBF_DST_ALPHA				= GL_DST_ALPHA,
			EBF_ONE_MINUS_DST_ALPHA		= GL_ONE_MINUS_DST_ALPHA,
		};

		enum EBlendOperator
		{
			EBOP_ADD,
			EBOP_SUB,
		};

		enum ECullFace
		{
			ECF_CCW = GL_CCW,
			ECF_CW	= GL_CW,
		};

	private:// embed class or struct or enum

	public:// method	
		CRenderState();
		virtual ~CRenderState();

		void	enableBlend(BOOLEAN bEnable);
		void	blendFunc(EBlendFactor eSrcFactor, EBlendFactor eDstFactor);
		void	blendOp(EBlendOperator eOp);

		void	enableCullFace(BOOLEAN bEnable);
		void	cullFace(ECullFace eCullFace);
		
		void	enableDepthTest(BOOLEAN bEnable);
		//void	depthFunc();

		UINT	getMaxTextureImageNum() const		{ return 8;	}

	protected:// method
		CRenderState(const CRenderState&)	{}
		CRenderState &operator = (const CRenderState &){return *this;}

	private:// method

	protected:// property

	private:// property
		BOOLEAN			m_bEnableBlend;
		EBlendFactor	m_eBlendSrcFactor;
		EBlendFactor	m_eBlendDstFactor;
		EBlendOperator	m_eBlendOp;

		BOOLEAN			m_bEnableCullFace;
		ECullFace		m_eCullFace;

		BOOLEAN			m_bEnableDepthTest;
	};
}