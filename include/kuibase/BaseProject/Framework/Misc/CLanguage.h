/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CLanguage	
	*			This is a shortcut class for accessing string table.
	*/
	class OPENHMI_API CLanguage
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		/**
		* Get string content by string ID.
		*/
		static	const String&	getString(UINT uiID);

	protected:// method
		CLanguage(){}
		CLanguage(const CLanguage&){}
		virtual ~CLanguage(){}		
		CLanguage& operator =(const CLanguage&){return *this;}	

	private:// method

	protected:// property

	private:// property

	};
}