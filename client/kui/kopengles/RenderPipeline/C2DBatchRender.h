#pragma once

namespace OpenHMI
{
	class C2DBatchRender : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		struct tagBatchRenderItem
		{
			//CRenderItem	*	m_pRndItem;
			FLOAT			m_fLeft;
			FLOAT			m_fTop;
			FLOAT			m_fRight;
			FLOAT			m_fBottom;
			FLOAT			m_fUVLeft;
			FLOAT			m_fUVTop;
			FLOAT			m_fUVRight;
			FLOAT			m_fUVBottom;
			FLOAT			m_fAlpha;
			FLOAT			m_fColor;

			tagBatchRenderItem() 
				: m_fLeft(0.0f)
				, m_fTop(0.0f)
				, m_fRight(0.0f)
				, m_fBottom(0.0f)
				, m_fUVLeft(0.0f)
				, m_fUVTop(0.0f)
				, m_fUVRight(0.0f)
				, m_fUVBottom(0.0f)
				, m_fAlpha(1.0f)
				, m_fColor(0.0f)
			{
				;
			}
		};
		
	private:// embed class or struct or enum

	public:// method	
		C2DBatchRender();
		virtual ~C2DBatchRender();

		void setParent(C2DBatchRenderGroup* pGroup)		{ m_pParent = pGroup;	}
		C2DBatchRenderGroup*	getParent()				{ return m_pParent;		}

		void addRenderItem(CRenderGroup* pRndGroup, BOOLEAN bOpengl);		
		void addRenderItem(const CRectangle& rcDest, const CRectangleF& rcTex, CCacheMaterial* pMtl);
		void setRenderModel(INT iX, INT iY, CCacheMesh* pMesh, CCacheMesh* pVBOMesh, CCacheMaterial* pMaterial);
	
		void merge(C2DBatchRender* pRender, CCacheMaterial* pMtl);
		void render(CDynamicUIMesh* pMesh, CCacheMesh* pVBOMesh);

		void reset();

		CRenderEntry*			getRenderEntry();

		BOOLEAN					getCanMerge(CRenderItem* pRndItem) const;
		BOOLEAN					getCanMerge(CCacheMaterial* pMtl) const;
		BOOLEAN					isCanMerged() const					{ return ( m_aryItems.size() > 0 ); }

		void					clone(C2DBatchRender* pSrc);

		BOOLEAN					is25D() const						{ return m_bIs25D;		}
		CCacheMaterial*			getMaterial() const					{ return m_pMaterial;	}
		const C3DMatrix&		getWorldMatrix() const				{ return m_worldMatrix;	}
		const CRectangle&		getbbox() const						{ return m_rcbbox;		}

	protected:// method
		C2DBatchRender(const C2DBatchRender&)	{}
		C2DBatchRender &operator = (const C2DBatchRender &){return *this;}	

		void genMesh(CDynamicUIMesh* pMesh);
		void bindAttributes(CCacheMesh*	pMesh, CCacheMesh* pVBOMesh);
		void unBindAttributes(CCacheMesh* pMesh);
		void draw(CCacheMesh* pMesh, CCacheMesh* pVBOMesh);

		CCacheMaterial* getMaterial(CRenderItem* pRndItem) const;

	private:// method
		void onAddRenderItem(const CRectangle& rcDest);

	protected:// property

	private:// property	
		Array2<tagBatchRenderItem>			m_aryItems;

		ObjectPtr<CCacheMesh>				m_pMesh;
		ObjectPtr<CCacheMesh>				m_pVBOMesh;
		C2DBatchRenderGroup*				m_pParent;	
		ObjectPtr<CCacheMaterial>			m_pMaterial;
		C3DMatrix							m_worldMatrix;
		BOOLEAN								m_bIs25D;
		CRectangle							m_rcbbox;
	};
}