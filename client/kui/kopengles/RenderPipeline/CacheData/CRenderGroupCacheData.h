#pragma once

#include "./RenderPipeline/C2DBatchRenderGroup.h"
namespace OpenHMI
{
	class CRenderGroupCacheData : public CMaterialRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderGroupCacheData(CRenderGroup*);
		virtual ~CRenderGroupCacheData();

		virtual	void		notifySizePropertyChanged()			{ m_bSizeChanged = TRUE;			}
		virtual	void		notifyMergePropertyChanged()		{ m_bMergePropertyChanged = TRUE;	}
		virtual	void		notifyContentPropertyChanged()		{ m_bContentPropertyChanged = TRUE;	}
		virtual void		notifyChildrenChanged()				{ m_bContentPropertyChanged = TRUE;	}
		
		void				flush( C2DBatchRenderGroup* pContainer);
		void				resetState();

		C2DBatchRenderGroup*getBatchRenderGroup()				{ return m_pBatchRenderGroup;	}
		CRenderGroup*		getRenderGroup()					{ return m_pRndGroup;			}

		BOOLEAN				getNeedUpdate()						{ return (m_bSizeChanged || m_bContentPropertyChanged); }
		CRenderTarget*		getRenderTarget();
		CCacheTexture*		getTexture();

		C3DMatrix&			getRotateZTransMatrix()				{ return m_rotateZTransMatrix;	}
	
		FLOAT				getRealWidth()	const				{ return m_fW;					}
		FLOAT				getRealHeight() const				{ return m_fH;					}
		FLOAT				getAnchorX()	const				{ return m_fAnchorX	;			}
		FLOAT				getAnchorY() const					{ return m_fAnchorY	;			}

		UINT				getFilterCount() const				{ return m_aryFilter.size();	}
		CBaseFilter*		getFilterAt(UINT uiIndex)			{ return m_aryFilter[uiIndex];	}

	protected:// method
		CRenderGroupCacheData(const CRenderGroupCacheData&)	{}
		CRenderGroupCacheData &operator = (const CRenderGroupCacheData &){return *this;}	

		void				flushMergeProperty(C2DBatchRenderGroup* pContainer);
		virtual void		flushMaterial();

	private:// method
		void				guardAnchor(FLOAT& fAnchor);
		void				flushFilter();

	protected:// property

	private:// property	
		CRenderGroup*		m_pRndGroup;
		C3DMatrix			m_rotateZTransMatrix;
		
		BOOLEAN				m_bSizeChanged;
		BOOLEAN				m_bMergePropertyChanged;
		BOOLEAN				m_bContentPropertyChanged;

		C3DVector			m_worldPos;
		FLOAT				m_fW;
		FLOAT				m_fH;
		FLOAT				m_fAnchorX;
		FLOAT				m_fAnchorY;
		C2DBatchRenderGroupPtr	m_pBatchRenderGroup;

		Array<CBaseFilter>	m_aryFilter;
	};
}