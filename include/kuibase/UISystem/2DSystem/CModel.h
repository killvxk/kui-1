/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CModel : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method		
		virtual ~CModel();

		void	setComponent(CComponent *pComponent)	{ m_pComponent = pComponent; }

		/**
		* Calling this method will cause the control to refresh its appearance.
		*/
		void	notifyModelChanged();

	protected:// method
		CModel();
		CModel(const CModel&){}
		CModel& operator =(const CModel&){return *this;}

	private:// method		

	protected:// property

	private:// property
		CComponent*		m_pComponent;
	};
}