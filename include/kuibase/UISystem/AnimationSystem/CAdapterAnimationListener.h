/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CAdapterAnimationListener
	*			It is an adapter class implementing interface IAnimationListener used for inherited.
	*/
	class OPENHMI_API CAdapterAnimationListener : public IAnimationListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CAdapterAnimationListener();

		virtual	void	onAnimationStarted(CBaseAnimation* pSource);
		virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		virtual	void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CAdapterAnimationListener();

	private:// method

	protected:// property

	private:// property

	};
}