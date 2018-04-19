/*@Author Zeki.Yan*/
#pragma once

#include "Common.h"

namespace OpenHMI
{
	/**
	* @class	Object	
	*			It is almost the base class of all classes in OpenHMI SDK.
	*/
	class OPENHMI_API Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~Object();

		/**
		*  Inner method.
		*/
		void	addRef();

		/**
		*  Inner method.
		*/
		void	release();

	protected:// method
		Object();		

	private:// method

	protected:// property
		INT		m_iRef;

	private:// property
		
	};
}