#pragma once

namespace OpenHMI
{
	class CRenderDelegate : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderDelegate(){}
		virtual ~CRenderDelegate(){}

		virtual void render(const Array2<INT>& aryUniformLoc, CCamera* pCamera, const C3DMatrix& modelMatrix, Object* pObject,  CRenderState* pRndState)	{}

	protected:// method
		CRenderDelegate(const CRenderDelegate&)	{}
		CRenderDelegate &operator = (const CRenderDelegate &){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}