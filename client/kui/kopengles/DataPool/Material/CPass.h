#pragma once

namespace OpenHMI
{
	class CPass
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CPass(CCacheMaterial* p);
		virtual ~CPass();

		void		setType(CMaterialPool::EPass_Type eType)		{ m_eType = eType;			}
		void		setRenderDelegate(CRenderDelegate* p)			{ m_pRenderDelegate = p;	}
		
		void		enableBlend(BOOLEAN bEnable);
		void		applyBlend(CRenderState::EBlendFactor eSrcFactor, CRenderState::EBlendFactor eDstFactor);
		void		enableCullFace(BOOLEAN bEnable);
		void		applyCullFace(CRenderState::ECullFace eCullFace);

		void		addTextureState(CMaterialPool::tagTextureState& rTexSate);
		void		setTextureStates(const Array2<CMaterialPool::tagTextureState>& ary);
		void		setProgram(CCacheProgram* p)			{ m_pProgram = p; }

		void		begin();
		void		end();
		void		apply(Object* p, CRenderState* pRndState);
		void		applyRenderStates(CRenderState* pRndState);
		void		applyProgram(Object* p);
		void		applyTextureParameters(UINT uidx, CCacheTexture* pTex);

		CCacheProgram*  getProgram()			{ return m_pProgram;	}

		CCacheTexture*	getTexture(UINT idx);

	protected:// method
		CPass(const CPass &){}
		CPass &operator = (const CPass &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		//INT				m_iProgramID;
		CCacheProgramPtr	m_pProgram;
		CCacheMaterial*		m_pParent;

		CMaterialPool::EPass_Type			m_eType;
		CRenderDelegate*					m_pRenderDelegate;

		BOOLEAN								m_bEnableBlend;
		CRenderState::EBlendFactor			m_eBlendSrcFactor;
		CRenderState::EBlendFactor			m_eBlendDstFactor;
		BOOLEAN								m_bEnableCullFace;
		CRenderState::ECullFace				m_eCullFace;

		Array2<CMaterialPool::tagTextureState>				m_aryTexState;

	};
}