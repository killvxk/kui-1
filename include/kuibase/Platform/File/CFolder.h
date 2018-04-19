/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CFolder	
	*			It is a common folder class.
	*/
	class OPENHMI_API CFolder : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		struct tagEntry
		{
			String	strPath;
			String	strName;
			BOOLEAN	bIsFolder;
		};

	private:// embed class or struct or enum

	public:// method
		CFolder();
		virtual ~CFolder();

		static	BOOLEAN				createFolder(const CHAR *pcPath);

		BOOLEAN						open(const CHAR *pcPath);
		void						close();
		UINT						getEntryCount();
		const CFolder::tagEntry*	getEntryByIndex(UINT uiIndex) const;

	protected:// method
		CFolder(const CFolder&){}
		CFolder& operator =(const CFolder&){return *this;}

	private:// method

	protected:// property

	private:// property
		Array<tagEntry>		m_aryEntry;
	};
}