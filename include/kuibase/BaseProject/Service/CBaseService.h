/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseService
		: public Object
		, public CEventDispatcher
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseService();

		DECLARE_SERVICE_METHOD_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		void	initialize();

		/**
		*  Inner method.
		*/
		void	unInitialize();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Invoke a service method specified by uiMethodID with parameter pData. In this function,the thread context will be switched.
		*
		* @param	uiMethodID	A ID used to specify the service method.
		* @param	pData	The pointer of the parameter package of the service method.
		* @return	A counter for count the times "invoke" function invoked.
		*/
		INT		invoke(UINT uiMethodID, Object *pData);

		/**
		* @brief	Dispatch a method response event to the main thread. In this function,the thread context will be switched.
		*
		* @param	uiMethodID	A ID used to specify the service method.
		* @param	pData		The pointer of the returned data package of the service method.
		*/
		void	dispatchMethodResponse(UINT uiMethodID, Object *pData);

		/**
		* @brief	Dispatch a signal event to the main thread. In this function,the thread context will be switched.
		*
		* @param	uiSignalID	A ID used to specify the service signal.
		* @param	pData		The pointer of the signal data package of the service signal.
		*/
		void	dispatchSignal(UINT uiSignalID, Object *pData);

		/**
		* @brief	Check the data is error or not. Each service should override this function to make this judgement.
		*
		* @param	pData	The data need to check, which is method return data from platform service.
		* @return	TRUE	An error happens.
		*			FALSE	No error happens.
		*/
		virtual BOOLEAN checkError(Object *pData) { return FALSE; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		void	dispatchEvent(UINT uiEventID, Object *pData);

	protected:// method
		CBaseService();		

	private:// method

		void	_initialize(Object *pData);

	protected:// property
		IBaseService	*m_pBaseServcie;

	private:// property
		
	};
}