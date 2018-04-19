/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CKeyTable_ASC : public CKeyTable
	{
	public:// const define
		/**
		 * key code enum
		 */
		static  const UINT	NUM_0				= 48;
		static  const UINT	NUM_1				= 49;
		static  const UINT	NUM_2				= 50;
		static  const UINT	NUM_3				= 51;
		static  const UINT	NUM_4				= 52;
		static  const UINT	NUM_5				= 53;
		static  const UINT	NUM_6				= 54;
		static  const UINT	NUM_7				= 55;
		static  const UINT	NUM_8				= 56;
		static  const UINT	NUM_9				= 57;
		
		static  const UINT	LET_A				= 65;
		static  const UINT	LET_B				= 66;
		static  const UINT	LET_C				= 67;
		static  const UINT	LET_D				= 68;
		static  const UINT	LET_E				= 69;
		static  const UINT	LET_F				= 70;
		static  const UINT	LET_G				= 71;
		static  const UINT	LET_H				= 72;
		static  const UINT	LET_I				= 73;
		static  const UINT	LET_J				= 74;
		static  const UINT	LET_K				= 75;
		static  const UINT	LET_L				= 76;
		static  const UINT	LET_M				= 77;
		static  const UINT	LET_N				= 78;
		static  const UINT	LET_O				= 79;
		static  const UINT	LET_P				= 80;
		static  const UINT	LET_Q				= 81;
		static  const UINT	LET_R				= 82;
		static  const UINT	LET_S				= 83;
		static  const UINT	LET_T				= 84;
		static  const UINT	LET_U				= 85;
		static  const UINT	LET_V				= 86;
		static  const UINT	LET_W				= 87;
		static  const UINT	LET_X				= 88;
		static  const UINT	LET_Y				= 89;
		static  const UINT	LET_Z				= 90;
		
		static  const UINT	SIG_SPACE			= 32;
		static  const UINT	SIG_HASH			= 35; // "#" or pound
		static  const UINT	SIG_PERCENT			= 37; // "%"
		static  const UINT	SIG_AND				= 38;
		static  const UINT	SIG_MULTIPLICATION	= 42;
		static  const UINT	SIG_ADDITION		= 43;
		static  const UINT	SIG_SUBTRACTION		= 45;
		static  const UINT	SIG_DOT				= 46;
		static  const UINT	SIG_DIVISION		= 47;
		static  const UINT	SIG_AT				= 64;				
		
		static  const UINT	FUN_BACKSPACE		= 8;
		static  const UINT	FUN_ENTER			= 13;		
		static  const UINT	FUN_DELETE			= 127;
		static  const UINT	FUN_CAPSLOCK		= 300;
		
		/**
		 * key type enum
		 */
		static  const UINT	KT_NUMBER			= 0;
		static  const UINT	KT_LETTER			= 1;
		static  const UINT  KT_SIGN				= 2;
		static  const UINT	KT_FUNC				= 3;

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CKeyTable_ASC();
		virtual ~CKeyTable_ASC();
		
		String  getKeyCodeCharacter(BOOLEAN bUseLowercase = FALSE);

		BOOLEAN isNumberKeyCode()   const { return m_uiKeyType == KT_NUMBER ? TRUE : FALSE; }
		BOOLEAN	isLetterKeyCode()   const { return m_uiKeyType == KT_LETTER ? TRUE : FALSE; }
		BOOLEAN isSignKeyCode()		const { return m_uiKeyType == KT_SIGN   ? TRUE : FALSE; }
		BOOLEAN isFuncKeyCode()		const { return m_uiKeyType == KT_FUNC   ? TRUE : FALSE; }

	protected:// method
		CKeyTable_ASC(const CKeyTable_ASC&){}
		CKeyTable_ASC& operator =(const CKeyTable_ASC&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}