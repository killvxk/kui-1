/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CBaseOperationCenter	
	*			It is an assistance class for CBaseMainframe, used to response to global signals and hard keys or commands..
	*/
	class OPENHMI_API CBaseOperationCenter
		: public Object
		//, public ICommandListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		virtual ~CBaseOperationCenter();		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Initialize OperationCenter (EG: set up connection with services).
		* The derived class should override this method.
		*/
		virtual	void	initialize() = 0;

		/**
		* Uninitialize OperationCenter (EG: break up connection with services).
		* The derived class should override this method.
		*/
		virtual	void	unInitialize() = 0;

		/**
		* @brief	Deal with the hard key event.
		*			The derived class should override this method.
		*			
		* @param	uiKeyCode	The key code for hard key, it is defined by general projects.
		* @param	iKeyStatus	The key status for hard key, it is defined by general projects.
		* @return	TRUE	Means the OperationCenter has deal with this hard key.
		*			FALSE	Means the OperationCenter has not deal with this hard key.
		*/
		virtual	void	onHardkey(UINT uiKeyCode, INT iKeyStatus);

		/**
		* @brief	Deal with the command event.
		*			The derived class should override this method.
		*			
		* @param	strCommand	The command name, it is defined by general projects.
		* @param	pData		The appended data of this command, it can be NULL.
		* @return	TRUE	Means the OperationCenter has deal with this command.
		*			FALSE	Means the OperationCenter has not deal with this command.
		*/
		virtual	BOOLEAN	onCommand(const String &strCommand, Object *pData = NULL);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//virtual	BOOLEAN	onDispatchCommand(const String &strCommand, Object *pData = NULL);

	protected:// method
		CBaseOperationCenter();		

	private:// method

	protected:// property

	private:// property

	};
}