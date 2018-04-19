#include "RenderEngine.h"

namespace OpenHMI
{
	typedef void (*Func_INT)(Object*, INT&);
	typedef void (*Func_ArrayINT)(Object*, Array2<INT>&);
	typedef void (*Func_FLOAT)(Object*, FLOAT&);
	typedef void (*Func_ArrayFLOAT)(Object*, Array2<FLOAT>&);
	typedef void (*Func_Vec3)(Object*, C3DVector&);
	typedef void (*Func_ArrayVec3)(Object*, Array2<C3DVector>&);
	typedef void (*Func_Vec4)(Object*, C3DVector4&);
	typedef void (*Func_ArrayVec4)(Object*, Array2<C3DVector4>&);
	//typedef void (*Func_Matrix3)(Object*, C3DMatrix3&);
	//typedef void (*Func_ArrayMatrix3)(Object*, Array2<C3DMatrix3>&);
	typedef void (*Func_Matrix4)(Object*, C3DMatrix&);
	typedef void (*Func_ArrayMatrix4)(Object*, Array2<C3DMatrix>&);


	#define CallFunc(FuncType,FuncPtr)	\
		FuncType func = (FuncType)(FuncPtr);	\
		if (func)	\
			func(pRender,ret);

	void setUniform_Int(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		INT ret = 0;
		CallFunc(Func_INT,nameCallBack);
		
		glUniform1i(iLoc,ret);
	}

	void setUniform_IntArray(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		Array2<INT> ret;
		
		CallFunc(Func_ArrayINT,nameCallBack);
		glUniform1iv(iLoc,ret.size(),&ret[0]);
	}

	void setUniform_Float(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		FLOAT ret = 0;
		
		CallFunc(Func_FLOAT,nameCallBack);
		glUniform1f(iLoc,ret);
	}

	void setUniform_FloatArray(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		Array2<FLOAT> ret;

		CallFunc(Func_ArrayFLOAT,nameCallBack);
		glUniform1fv(iLoc,ret.size(),&ret[0]);
	}

	void setUniform_Vec3(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		C3DVector ret;
		
		CallFunc(Func_Vec3,nameCallBack);
		glUniform3f(iLoc,ret.getX(),ret.getY(),ret.getZ());
	}

	void setUniform_Vec3Array(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		Array2<C3DVector> ret;
		
		CallFunc(Func_ArrayVec3,nameCallBack);
		glUniform3fv(iLoc,ret.size(),(const GLfloat*)&ret[0]);
	}

	void setUniform_Vec4(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		C3DVector4 ret ;
		
		CallFunc(Func_Vec4,nameCallBack);
		glUniform4f(iLoc,ret.getX(),ret.getY(),ret.getZ(),ret.getW() );
	}

	void setUniform_Vec4Array(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		Array2<C3DVector4> ret;
		
		CallFunc(Func_ArrayVec4,nameCallBack);
		glUniform4fv(iLoc,ret.size(),(const GLfloat*)&ret[0]);
	}

	void setUniform_Matrix3(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		//C3DMatrix3 ret;
		//
		//CallFunc(Func_Matrix3,nameCallBack);
		//glUniformMatrix3fv(iLoc,1,GL_FALSE,(const GLfloat*)&ret);
	}

	void setUniform_Matrix3Array(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		//Array2<C3DMatrix3> ret;
		//
		//CallFunc(Func_ArrayMatrix3,nameCallBack);
		//glUniformMatrix3fv(iLoc,ret.size(),GL_FALSE,(const GLfloat*)&ret[0]);
	}

	void setUniform_Matrix4(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		C3DMatrix ret;
		
		CallFunc(Func_Matrix4, nameCallBack);
		glUniformMatrix4fv(iLoc, 1, GL_FALSE, (const GLfloat*)ret.ptr());
	}

	void setUniform_Matrix4Array(Object* pRender,Uniform_NameCallBackFunc nameCallBack,INT iLoc)
	{
		Array2<C3DMatrix> ret;
		
		CallFunc(Func_ArrayMatrix4,nameCallBack);
		glUniformMatrix4fv(iLoc,ret.size(),GL_FALSE,(const GLfloat*)&ret[0]);
	}

	//--------------------------------------------------------------------------------------------------
	void getProject_UI(Object* p,C3DMatrix& ret)
	{
		C3DMatrix* pMtRotateZ = NULL;
		
		C2DBatchRender* pRender = (C2DBatchRender*)p;
		C2DBatchRenderGroup* pBatchRndGroup = pRender->getParent();
		
		const C3DMatrix& mtProject = pBatchRndGroup->getProjectMatrix(FALSE);
		ret = mtProject;
	}

	void getMP_UI(Object* p,C3DMatrix& ret)
	{
		C2DBatchRender* pRender = (C2DBatchRender*)p;
		C2DBatchRenderGroup* pBatchRndGroup = pRender->getParent();
		BOOLEAN b25D = pRender->is25D();
		const C3DMatrix& mtProject = pBatchRndGroup->getProjectMatrix( b25D );
		ret = mtProject * pRender->getWorldMatrix();
		
	}
}