#pragma once

namespace OpenHMI
{
	class CFilterCenter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		/**
		* @brief	create filter
		* @parma	pFilter this is the source RenderFilter
		*/
		static CBaseFilter* createFilter(const CRenderBaseFilter* pFilter);

		static void			refreshFilterGenerateRT(const Array<CBaseFilter>& aryFilter);

	protected:// method
		CFilterCenter(){}
		CFilterCenter(const CFilterCenter&){}
		virtual ~CFilterCenter(){}		
		CFilterCenter& operator =(const CFilterCenter&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}