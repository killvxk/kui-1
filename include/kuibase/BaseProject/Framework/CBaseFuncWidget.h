/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CBaseFuncWidget
	*			This class is the base class for all the project FuncWidgets.
	*/
	class OPENHMI_API CBaseFuncWidget 
		: public CContainer
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseFuncWidget();

		DECLARE_EVENT_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	After this FuncWidget created, it should be initialized using this function.
		*			The derived class should override this method.
		*			
		* @return	TRUE	Means this FuncWidget is ready to enter.
		*			FALSE	Means this FuncWidget is not able to enter.
		*/
		virtual BOOLEAN	initialize() = 0;

		/**
		* Before this FuncWidget is destroyed, it should be uninitialized using this function.
		* The derived class should override this method.
		*/
		virtual void	unInitialize() = 0;

		/**
		* Initialize skin info and set data to children controls or set children controls status according to the logic data.
		* After calling this method, the refresh method will be called automatically.
		* The derived class should override this method.
		*/
		virtual	void	pushDataToUI() = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	void	refresh();
		virtual	void	destroy();

	protected:// method
		CBaseFuncWidget();		

	private:// method

	protected:// property

	private:// property

	};
}