#pragma once

namespace OpenHMI
{
	class CBatchRenderGroup
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CBatchRenderGroup();
		virtual ~CBatchRenderGroup();

		//virtual void			setRenderTarget(CRenderTarget* pRT);
		virtual CRenderTarget*	getRenderTarget();

		virtual void			render();
		virtual void			flush(BOOLEAN bGenerateRT = FALSE);
		virtual void			clear();

		virtual void			reset();

		CRenderEntry*			getOwner()										{ return m_pOwner;		}	
		void					setOwner(CRenderEntry* pOwner)					{ m_pOwner = pOwner;	}

		void					set(INT iX, INT iY, UINT uiW, UINT uiH);
		void					setX(INT iValue);
		INT						getX() const									{ return m_iX;			}
		void					setY(INT iValue);
		INT						getY() const									{ return m_iY;			}
		void					setWidth(UINT uiValue);
		UINT					getWidth() const								{ return m_uiWidth;		}
		void					setHeight(UINT uiValue);
		UINT					getHeight() const								{ return m_uiHeight;	}

		void					setParent(CBatchRenderGroup* pParent)			{ m_pParent = pParent;	}
		CBatchRenderGroup*		getParent()										{ return m_pParent;		}

		void					addRef();
		virtual void			release();

	protected:// method
		CBatchRenderGroup(const CBatchRenderGroup&)	{}
		CBatchRenderGroup &operator = (const CBatchRenderGroup &){return *this;}	

		BOOLEAN	getPositionChanged() const	{ return m_bPosChanged;	}
		void	notifyPositionChanged()		{ m_bPosChanged = TRUE; }

		BOOLEAN getSizeChanged() const		{ return m_bSizeChanged;}
		void	notifySizeChanged()			{ m_bSizeChanged = TRUE;}

	private:// method
		
	protected:// property
		INT				m_iRef;
		INT				m_iX;
		INT				m_iY;
		UINT			m_uiWidth;
		UINT			m_uiHeight;
		CBatchRenderGroup*	m_pParent;

	private:// property
		CRenderEntry*	m_pOwner;
		CRenderTargetPtr	m_pRT;

		BOOLEAN			m_bPosChanged;
		BOOLEAN			m_bSizeChanged;
	};

	typedef ObjectPtr<CBatchRenderGroup>	CBatchRenderGroupPtr;
	
}