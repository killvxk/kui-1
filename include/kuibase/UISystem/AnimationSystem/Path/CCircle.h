/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CCircle : public CBasePath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CCircle();
		virtual ~CCircle();

		virtual	void smashToSegments(INT iSegmentCount);

	protected:// method
		CCircle(const CCircle&){}
		CCircle& operator =(const CCircle&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}