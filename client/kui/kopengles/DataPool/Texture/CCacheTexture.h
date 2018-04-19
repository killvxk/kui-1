#pragma once

namespace OpenHMI
{
	class CCacheTexture : public CacheGPUObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ETexType
		{
			TEXTYPE_2D			= GL_TEXTURE_2D,
		};

		enum ETexFormat
		{
			TEXFMT_RGBA8888		= 0x00000001,
			TEXFMT_RGB888		= 0x00000002,
			TEXFMT_RGB565		= 0x00000004,
			TEXFMT_RGBA5551		= 0x00000008,	
			TEXFMT_ALPHA		= 0x00000010,
			TEXFMT_LUMINANCE	= 0x00000020,
		};

		enum ETexUsage
		{
			TEXUSAGE_NULL		= 0x00001000,
			TEXUSAGE_TEXTMERGE	= 0x00010000,
		};

		enum EWrapParameter 
		{
            WRAP_S,
            WRAP_T,
            WRAP_R
        };

        enum EWrapMode 
		{
            WRAPMODE_CLAMP_TO_EDGE	= GL_CLAMP_TO_EDGE,
            WRAPMODE_REPEAT			= GL_REPEAT,
        };

		enum EFilterParameter 
		{
            FP_MIN_FILTER,
            FP_MAG_FILTER
        };

        enum EFilterMode 
		{
            FM_LINEAR                    = GL_LINEAR,
            FM_LINEAR_MIPMAP_LINEAR      = GL_LINEAR_MIPMAP_LINEAR,
            FM_LINEAR_MIPMAP_NEAREST     = GL_LINEAR_MIPMAP_NEAREST,
            FM_NEAREST                   = GL_NEAREST,
            FM_NEAREST_MIPMAP_LINEAR     = GL_NEAREST_MIPMAP_LINEAR,
            FM_NEAREST_MIPMAP_NEAREST    = GL_NEAREST_MIPMAP_NEAREST
        };

	private:// embed class or struct or enum

	public:// method	
		CCacheTexture();
		~CCacheTexture();

		void		setWidth(INT w)				{ m_uiWidth = w;		}
		INT			getWidth() const			{ return m_uiWidth;		}
		void		setHeight(INT h)			{ m_uiHeight = h;		}
		INT			getHeight() const			{ return m_uiHeight;	}

		void		setFormat(UINT format)		{ m_uiFormat = format;	}
		UINT		getFormat() const			{ return m_uiFormat;	}

		void		setType(UINT type)			{ m_uiType = type;		}
		UINT		getType() const				{ return m_uiType;		}

		// texture usage may be GPU related parameter ,now just usage
		void		setUsage(UINT usage)		{ m_uiUsage = usage;	}
		UINT		getUsage() const			{ return m_uiUsage;		}
		BOOLEAN		isTextMergeTexture() const	{ return (m_uiUsage & TEXUSAGE_TEXTMERGE) != 0;}

		EWrapMode	getWrap(EWrapParameter eWhich) const;
		void		setWrap(EWrapParameter eWhich, EWrapMode eMode);

		EFilterMode	getFilter(EFilterParameter eWhich) const;
		void		setFilter(EFilterParameter eWhich, EFilterMode eMode);

		UINT		getTexID() const			{ return m_uiTexId;		}

		void		generateGPUResource(const BYTE* pData);
		void		fill(const BYTE* pData);
		void		fill(INT uiXOffset, INT uiYOffset, INT uiWidth, INT uiHeight, const BYTE* pData);

		void		applyTextureParameters();

		static void TransferTexFormat(UINT format, GLint& internalFormat, GLenum& type);

	protected:// method
		CCacheTexture(const CCacheTexture &){}
		CCacheTexture &operator = (const CCacheTexture &){return *this;}

		void			dirtyTextureParameters(){ m_bNeedUpdateParameter = TRUE;	}
	
		virtual void	onGenerateGPUResource()	{}

	private:// method

	protected:// property
		INT			m_uiWidth;
		INT			m_uiHeight;
		UINT		m_uiFormat;		// RGB888 RGB556 RGGA8888
		UINT		m_uiType;		// 2D Texture
		UINT		m_uiUsage;		// texture usage
		UINT		m_uiTexId;

		EWrapMode	m_wrap_s;
        EWrapMode	m_wrap_t;
        EWrapMode	m_wrap_r;

        EFilterMode m_min_filter;
        EFilterMode m_mag_filter;
		
		BOOLEAN		m_bNeedUpdateParameter;

	private:// property	
		
	};

	typedef ObjectPtr<CCacheTexture> CCacheTexturePtr;
}