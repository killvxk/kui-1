#pragma once

namespace OpenHMI
{
	class CacheGPUObject : public CacheObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum


	private:// embed class or struct or enum

	public:// method	
		CacheGPUObject();
		virtual ~CacheGPUObject();

		void				setRenderContext(tagRenderContext* p)	{ m_pRenderContext = p;		}
		tagRenderContext*	getRenderContext()						{ return m_pRenderContext;	}

		void				changeRenderContext();
		void				resetRenderContext();

	protected:// method
		CacheGPUObject(const CacheGPUObject &){}
		CacheGPUObject &operator = (const CacheGPUObject &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		tagRenderContext*	m_pRenderContext;

	};
}