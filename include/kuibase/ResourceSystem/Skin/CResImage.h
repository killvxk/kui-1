/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CResImage : public CResNode
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CResImage();
		virtual ~CResImage();

		UINT			getImageSize();
		String			getImage(UINT uiIndex) const;
		void			addImage(const String &strPath);
		void			deleteImage(UINT uiIndex);		
		void			replaceImage(UINT uiIndex, const String &strPath);

		void			setAsCommonImage();
		BOOLEAN			isCommomImage() const { return m_bIsCommonImage; }

		virtual void	parseProperty(const String &strData);
		virtual String	saveProperty();

	protected:// method
		CResImage(const CResImage &){}
		CResImage &operator = (const CResImage &){return *this;}	

		virtual	void	notifyPropertyChanged();

	private:// method
		void	getImageList();
		void	updateImageProperty();

	protected:// property

	private:// property
		Array<String> m_aryImage;

		BOOLEAN		m_bIsCommonImage;
	};
}