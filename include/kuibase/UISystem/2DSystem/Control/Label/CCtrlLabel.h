/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CCtrlLabel : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlLabel();
		virtual ~CCtrlLabel();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set the label's caption.
		*/
		void			setCaption(const String &strValue);
		const String&	getCaption();

		/**
		* @brief	Set the label's caption when the label has multi-captions.
		*
		* @param	strName		The text field name of the caption, defined in OpenHMIArtist.
		* @param	strValue	The content of the caption.
		*/
		void			setCaption(const String &strName, const String &strValue);
		const String&	getCaption(const String &strName) const;
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Get the text field which contains the the label's caption.
		*/
		CCtrlTextField*	getTextField();

		/**
		* @brief	Get the text field when the label has multi-captions.
		*
		* @param	strName The text field name defined in OpenHMIArtist.
		* @return	a pointer to the text field.
		*/
		CCtrlTextField*	getTextField(const String &strName);

		/**
		* Get the image field which contains the label's icon.
		*/
		CCtrlImageField*	getImageField();

		/**
		* @brief	Get the image field when the label has multi-icons.
		*
		* @param	strName The image field name defined in OpenHMIArtist.
		* @return	a pointer to the image field.
		*/
		CCtrlImageField*	getImageField(const String &strName);

		/**
		*  Inner method.
		*/
		CCtrlTextField*		extractTextField(const String &strName);

		/**
		*  Inner method.
		*/
		CCtrlImageField*	extractImageField(const String &strName);

		virtual	void	refresh();
		virtual	void	littleRefresh();
		virtual	void	destroySkin();
		virtual	void	destroy();
		virtual void	buildRenderTree(CRenderGroup &renderGroup);

	protected:// method
		CCtrlLabel(const CCtrlLabel&){}
		CCtrlLabel& operator =(const CCtrlLabel&){return *this;}

		void			updateView();
	private:// method

	protected:// property
		Map<String, CCtrlTextField, tagStringCompare>	m_mapTextField;
		Map<String, CCtrlImageField, tagStringCompare>	m_mapImageField;

		BOOLEAN			m_bLongFocus;

	private:// property
		CViewLabel			m_view;
	};
}