/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBezier : public CBasePath
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CBezier();
		virtual ~CBezier();

		virtual	void smashToSegments(INT iSegmentCount);

	protected:// method
		CBezier(const CBezier&){}
		CBezier& operator =(const CBezier&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}