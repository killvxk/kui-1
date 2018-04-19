#pragma once

namespace OpenHMI
{
	class CWaveRenderDelegate : public CRenderDelegate
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CWaveRenderDelegate();
		virtual ~CWaveRenderDelegate();

		void		 initialize();
		void		 setTexture(CCacheTexture* pTex);

		virtual void render(const Array2<INT>& aryUniformLoc, CCamera* pCamera, const C3DMatrix& modelMatrix, Object* pObject, CRenderState* pRndState);

	protected:// method
		CWaveRenderDelegate(const CWaveRenderDelegate&)	{}
		CWaveRenderDelegate &operator = (const CWaveRenderDelegate &){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT		m_uiTexID;

	};
}