/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CManager_Logic
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method		
		static	CManager_Logic*	getInstance();
		static	void			deleteInstance();

		/**
		*  Inner method.
		*/
		void		initialize();

		/**
		*  Inner method.
		*/
		void		unInitialize();

		/**
		* @brief	Register a new logic class to CManager_Logic.
		*
		* @param	strClassName	A name used to indicate a logic class.
		* @param	pLogic			The pointer of a logic class.
		*/
		void		registerLogic(const String &strClassName, CBaseLogic *pLogic);

		/**
		*  Inner method.
		*/
		CBaseLogic*	getLogicByName(const String &strClassName);

	protected:// method
		CManager_Logic();
		virtual ~CManager_Logic();

	private:// method

	protected:// property

	private:// property
		static	CManager_Logic	*s_pInstance;

		Map<String, CBaseLogic, tagStringCompare>	m_mapLogic;
	};
}