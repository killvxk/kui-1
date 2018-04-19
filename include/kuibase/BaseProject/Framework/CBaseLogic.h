/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseLogic 
		: public Object
		, public CEventDispatcher
		, public IEventListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~CBaseLogic();
				
		DECLARE_EVENT_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Initialize the logic for an application.
		* When an application started, its logic should be initialized (EG: data /connection with service).
		* The derived class should override this method.
		*/
		virtual	BOOLEAN	initialize();

		/**
		* The logic for an application should be uninitialized (EG: release data/connection with service).
		* The derived class should override this method.
		*/
		virtual	void	unInitialize();

		/**
		* @brief	The property indicates whether the logic is ready or not.
		*			The derived class should override this method.
		*			
		* @return	TRUE	Means this logic is ready to provide data and function to UI.
		*			FALSE	Means this logic is not able to provide data and function to UI.
		*/
		virtual BOOLEAN	isReady();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Dispatch logic event to UI.
		*/
		void	dispatchEvent(UINT uiEventID);

	protected:// method
		CBaseLogic();		

	private:// method

	protected:// property	

	private:// property
		BOOLEAN		m_bInitialized;
	};
}