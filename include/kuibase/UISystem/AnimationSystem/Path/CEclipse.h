/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEclipse : public CBasePath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CEclipse();
		virtual ~CEclipse();

		virtual	void smashToSegments(INT iSegmentCount);

	protected:// method
		CEclipse(const CEclipse&){}
		CEclipse& operator =(const CEclipse&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}