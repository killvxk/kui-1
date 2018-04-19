/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CRenderCacheData : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum		

	public:// method		
		CRenderCacheData();
		virtual ~CRenderCacheData();
		
		// size: width, height
		virtual	void	notifySizePropertyChanged()		{ }
		// : x, y, alpha, rotate, scale, etc
		virtual	void	notifyMergePropertyChanged()	{ }
		// content: text, font, image path, filter, etc
		virtual	void	notifyContentPropertyChanged()	{ }

		virtual void	notifyChildrenChanged()			{ }

	protected:// method
		CRenderCacheData(const CRenderCacheData&){}
		CRenderCacheData& operator =(const CRenderCacheData&){return *this;}
		
	private:// method

	protected:// property

	private:// property

	};
}