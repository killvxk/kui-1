#pragma once

namespace OpenHMI
{
	class CImgPNG
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method		
		static	CImgPNG*	getInstance();
		static	void		deleteInstance();

		CImageParser::tagImageInfo decodeFromFile(const String &strFilePath);

	protected:// method
		CImgPNG();
		virtual ~CImgPNG();

	private:// method

	protected:// property

	private:// property
		static	CImgPNG	*s_pInstance;

#ifdef OS_WIN

#endif

#ifdef OS_QNX
		img_lib_t m_imglib;
#endif

#ifdef OS_LINUX

#endif
	};
}