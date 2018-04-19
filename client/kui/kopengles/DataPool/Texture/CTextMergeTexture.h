#pragma once

namespace OpenHMI
{
	class CTextMergeTexture : public CCacheTexture
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CTextMergeTexture(UINT uiCellWidth, UINT uiCellHeight);
		~CTextMergeTexture();

		void			fill(UINT uidx, CDIB* pDIB);
		void			fill(const Array2<UINT>& aryIdx, CDIB* pDIB);
		void			free(UINT uidx);
		void			free(const Array2<UINT>& aryIdx);
		UINT			getFreeRect() const;
		BOOLEAN			getFreeRect(UINT uiWidth, UINT uiHeight, Array2<UINT>& aryIdx) const;
		CRectangleF		getROI(UINT uidx, UINT uiWidth, UINT uiHeight) const;

	protected:// method
		//CTextMergeTexture(const CTextMergeTexture &){}
		//CTextMergeTexture &operator = (const CTextMergeTexture &){return *this;}

		virtual void	onGenerateGPUResource();

	private:// method
		void		dirty(UINT uidx);
		void		dirty(const Array2<UINT>& aryIdx);
		BOOLEAN		isValid(UINT uidx) const;
		void		getOffset(UINT uidx, INT& iOffsetX, INT& iOffsetY) const;
		BOOLEAN		isFree(UINT uiStartCol, UINT uiStartRow, UINT uiColNum, UINT uiRowNum) const;

	protected:// property

	private:// property	
		Array2<BOOLEAN>	m_aryRectFreeFlag;
		UINT			m_uiCellWidth;
		UINT			m_uiCellHeight;
		UINT			m_uiCellRow;
		UINT			m_uiCellCol;

	};

	typedef ObjectPtr<CTextMergeTexture> CTextMergeTexturePtr;
}