/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseApplication : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		virtual ~CBaseApplication();

		/**
		* @brief	Enter into some screen according to the start params.
		*			The derived class should override this method.
		*			
		* @param	mapCommand	Define start params.
		*/
		virtual	void	start(const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand) = 0;

		/**
		* Close all opened screens.
		* The derived class should override this method.
		*/
		virtual	void	close() = 0;

	protected:// method
		CBaseApplication();		

	private:// method

	protected:// property

	private:// property

	};
}