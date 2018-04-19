/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventKeyboard
	{
	public:
		static const UINT KEY_CLICKED			= 0;
		static const UINT KEY_LONG_CLICKED		= 1;
		static const UINT KEY_CONTINUE_CLICKED	= 2;
	};

	class OPENHMI_API CWgtKeyboard 
		: public CContainer
		, public IEventListenerChain
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	protected: // embed class or struct or enum
		struct tagKey
		{
			String          strKey;
			UINT            uiLen;
			UINT            auiKeyType[5];
			UINT            auiKeyCode[5];
		};

	private:// embed class or struct or enum
		class CWgtKeyboard_Key
		{
		public:
			String          strKey;
			Array2<UINT>	aryKeyType;
			Array2<UINT>	aryKeyCode;

			CCtrlButton		btnKey;	
			INT				iTouchTime;			
			INT				iCurIndex;
		};

	public:// method
		~CWgtKeyboard();

		DECLARE_EVENT_MAP

		/**
		* Enable or disable the key by the key name defined in the OpenHMIArtist.
		*/
		void	setKeyEnable(const String& strKey, BOOLEAN bEnable);

		/**
		* Enable or disable the key by the key code.
		*/
		void	setKeyEnable(UINT uiKeyCode, BOOLEAN bEnable);

		/**
		* Set the key's focus status by the key name defined in the OpenHMIArtist.
		*/
		void	setKeyTouchFocus(const String& strKey, BOOLEAN bFocus);

		/**
		* Set the key's focus status by the key code.
		*/
		void	setKeyTouchFocus(UINT uiKeyCode, BOOLEAN bFocus);

		/**
		* Set the key's visible status by the key name defined in the OpenHMIArtist
		*/
		void	setKeyVisible(const String& strKey, BOOLEAN bVisible);

		/**
		* set the key's visible status by the key code.
		*/
		void	setKeyVisible(UINT uiKeyCode, BOOLEAN bVisible);

		/**
		* Set all key's visible status.
		*/
		void	setAllKeysVisible(BOOLEAN bVisible);

		CKeyTable*		getKeyTable() { return m_pKeyTable; }

		/**
		* Get a key button by the key name defined in the OpenHMIArtist.
		*/
		CCtrlButton*	getButton(const String& strKey);

		/**
		* Set the keyboard's enable status.
		*/
		void			setEnable(BOOLEAN bValue);
		virtual void	destroy();

		virtual	ICursorTracker*	probe(INT iX, INT iY);
    	virtual void			doFocusEnterIn();

		// touch event
		virtual	void	onTouchPress(INT iX, INT iY);
		virtual void	onTouchPressRelease(INT iX, INT iY);
		virtual void	onTouchLongPress(INT iX, INT iY);
		virtual void	onTouchLongPressRelease(INT iX, INT iY);
		virtual void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);
		virtual void	onTouchDrag(INT iX, INT iY);
		virtual void	onTouchDragRelease(INT iX, INT iY);

		// implement IEventListenerChain interface
		virtual BOOLEAN	contains(Object *pTarget);

	protected:// method
		CWgtKeyboard();
		CWgtKeyboard(const CWgtKeyboard&){}
		CWgtKeyboard& operator =(const CWgtKeyboard&){return *this;}

		void	registerKeyTable(CKeyTable* pKeyTable);
		void	addKey(const tagKey* pKeyTable, UINT uiKeyTableLength);

		/**
		 * Maybe the inherited classes should override this method for preparing new key table.
		 */
		virtual void buildKeyTable() = 0;
		
		/**
		 * Maybe the inherited classes should override this method for special effect.
		 */
		virtual void onKeyPressed(const String& strKey)		{}

		/**
		 * Maybe the inherited classes should override this method for special effect.
		 */
		virtual void onKeyReleased()						{}

		/**
		 * Maybe the inherited classes should override this method for special effect.
		 */
		virtual void onKeyLongPressed(const String& strKey) {}

		/**
		 * Maybe the inherited classes should override this method for special effect.
		 */
		virtual void onKeyLongReleased()					{}

		/**
		 * Maybe the inherited classes should override this method for special effect.
		 */
		virtual void onKeyFocusChanged(const String& strKey){}

	private:// method
		void	onBtnKeyClick(CComponent *pTarget);
		void	onBtnKeyLongClick(CComponent *pTarget);

		void	dispatchKeyboardEvent(UINT uiEventID, UINT uiKeyType, UINT uiKeyCode);

	protected:// property

	private:// property
		CKeyTable			*m_pKeyTable;
		CWgtKeyboard_Key	*m_pCurFocusKey;
		CWgtKeyboard_Key	*m_pNewFocusKey;
		BOOLEAN				m_bFocusChanged;

		Map<String, CWgtKeyboard_Key, tagStringCompare>  m_mapKey;
	};
}