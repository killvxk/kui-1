#pragma once

namespace OpenHMI
{
	class CFontPool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagFaceKey
		{
			String	strName;
			UINT	uiSize;		

			INT operator()(const tagFaceKey &key1, const tagFaceKey &key2) const
			{
				INT iRet = key1.strName.compare(key2.strName);

				if (iRet < 0)
				{
					return -1;
				}
				else if (iRet == 0)
				{
					if ( key1.uiSize == key2.uiSize )
					{
						return 0;
					}
					else if( key1.uiSize > key2.uiSize )
					{
						return 1;
					}
					else
					{
						return -1;
					}
				}
				else
				{
					return 1;
				}
			}
		};

	public:// method	
		static	CFontPool*	getInstance();
		static	void		deleteInstance();

		void		initialize();
		void		unInitialize();
		void		clear();

		CFontFace*	getFontFace(const String &strFontName, UINT uiFontSize);
		FT_Library	getFontLib() { return m_ftLib; };

	protected:// method	
		CFontPool();
		virtual ~CFontPool();

	private:// method		

	protected:// property

	private:// property
		static	CFontPool	*s_pInstance;

		FT_Library m_ftLib;

		Map<tagFaceKey, CFontFace, tagFaceKey> m_mapFace;
	};
}