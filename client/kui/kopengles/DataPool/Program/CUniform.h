#pragma once

namespace OpenHMI
{
	enum EUniformType
	{
		EUNTYPE_INT,
		EUNTYPE_INTARRAY,
		EUNTYPE_FLOAT,
		EUNTYPE_FLOATARRAY,
		EUNTYPE_VEC3,
		EUNTYPE_VEC3ARRAY,
		EUNTYPE_VEC4,
		EUNTYPE_VEC4ARRAY,
		EUNTYPE_MATRIX3,
		EUNTYPE_MATRIX3ARRAY,
		EUNTYPE_MATRIX4,
		EUNTYPE_MATRIX4ARRAY,

		EUNTYPE_NUM
	};

	enum EUniformName
	{
		EUNNAME_PROJECT_UI,
		EUNNAME_MP_UI,

	
		EUNNAME_NUM
	};

	typedef void*	Uniform_NameCallBackFunc; 
	typedef void	(*Uniform_TypeCallBackFunc)(Object* p,Uniform_NameCallBackFunc nameCallBackFunc, INT iLoc);

	// type call back func
	extern void setUniform_Int(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);
	extern void setUniform_IntArray(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);

	extern void setUniform_Float(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);
	extern void setUniform_FloatArray(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);

	extern void setUniform_Vec3(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);
	extern void setUniform_Vec3Array(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);

	extern void setUniform_Vec4(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);
	extern void setUniform_Vec4Array(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);

	extern void setUniform_Matrix3(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);
	extern void setUniform_Matrix3Array(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);

	extern void setUniform_Matrix4(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);
	extern void setUniform_Matrix4Array(Object* pRender, Uniform_NameCallBackFunc func, INT iLoc);

	// name call back func
	extern void getProject_UI(Object* p,C3DMatrix& ret);
	extern void getMP_UI(Object* p,C3DMatrix& ret);



}