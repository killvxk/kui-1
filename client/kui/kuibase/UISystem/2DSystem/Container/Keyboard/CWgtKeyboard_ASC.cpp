#include "OpenHMI.h"

namespace OpenHMI
{
	CWgtKeyboard_ASC::CWgtKeyboard_ASC(BOOLEAN bNumberBindLetter)
	{
		m_bNumberBindLetter = bNumberBindLetter;

		// must call this method.
		registerKeyTable(&m_keyTableASC);

		buildKeyTable();
	}

	CWgtKeyboard_ASC::~CWgtKeyboard_ASC()
	{
		;
	}

	void CWgtKeyboard_ASC::buildKeyTable()
	{
		if (m_bNumberBindLetter)
		{
			tagKey aKeyTable[50] = 
			{	
				{ L"mNum0", 1,	{CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_0} },
				{ L"mNum1", 1,	{CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_1} },
				{ L"mNum2", 4,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_2,		CKeyTable_ASC::LET_A,		CKeyTable_ASC::LET_B,		CKeyTable_ASC::LET_C} },
				{ L"mNum3", 4,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_3,		CKeyTable_ASC::LET_D,		CKeyTable_ASC::LET_E,		CKeyTable_ASC::LET_F} },
				{ L"mNum4", 4,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_4,		CKeyTable_ASC::LET_G,		CKeyTable_ASC::LET_H,		CKeyTable_ASC::LET_I} },
				{ L"mNum5", 4,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
					 			{CKeyTable_ASC::NUM_5,		CKeyTable_ASC::LET_J,		CKeyTable_ASC::LET_K,		CKeyTable_ASC::LET_L} },
				{ L"mNum6", 4,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_6,		CKeyTable_ASC::LET_M,		CKeyTable_ASC::LET_N,		CKeyTable_ASC::LET_O} },
				{ L"mNum7", 5,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_7,		CKeyTable_ASC::LET_P,		CKeyTable_ASC::LET_Q,		CKeyTable_ASC::LET_R,		CKeyTable_ASC::LET_S} },
				{ L"mNum8", 4,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_8,		CKeyTable_ASC::LET_T,		CKeyTable_ASC::LET_U,		CKeyTable_ASC::LET_V} },
				{ L"mNum9", 5,	{CKeyTable_ASC::KT_NUMBER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER,	CKeyTable_ASC::KT_LETTER}, 
								{CKeyTable_ASC::NUM_9,		CKeyTable_ASC::LET_W,		CKeyTable_ASC::LET_X,		CKeyTable_ASC::LET_Y,		CKeyTable_ASC::LET_Z} },

				{ L"mLetA", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_A} },
				{ L"mLetB", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_B} },
				{ L"mLetC", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_C} },
				{ L"mLetD", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_D} },
				{ L"mLetE", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_E} },
				{ L"mLetF", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_F} },
				{ L"mLetG", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_G} },
				{ L"mLetH", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_H} },
				{ L"mLetI", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_I} },
				{ L"mLetJ", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_J} },
				{ L"mLetK", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_K} },
				{ L"mLetL", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_L} },
				{ L"mLetM", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_M} },
				{ L"mLetN", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_N} },
				{ L"mLetO", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_O} },
				{ L"mLetP", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_P} },
				{ L"mLetQ", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_Q} },
				{ L"mLetR", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_R} },
				{ L"mLetS", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_S} },
				{ L"mLetT", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_T} },
				{ L"mLetU", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_U} },
				{ L"mLetV", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_V} },
				{ L"mLetW", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_W} },
				{ L"mLetX", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_X} },
				{ L"mLetY", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_Y} },
				{ L"mLetZ", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_Z} },

				{ L"mSignSpace",		  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_SPACE} },	
				{ L"mSignHash", 		  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_HASH} },
				{ L"mSignPercent",		  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_PERCENT} },
				{ L"mSignAnd", 			  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_AND} },
				{ L"mSignMultiplication", 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_MULTIPLICATION} },
				{ L"mSignAddition", 	  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_ADDITION} },
				{ L"mSignSubtraction", 	  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_SUBTRACTION} },
				{ L"mSignDot", 			  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_DOT} },
				{ L"mSignDivision", 	  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_DIVISION} },
				{ L"mSignAt", 			  1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_AT} },			

				{ L"mFuncBackspace",	  1, {CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_BACKSPACE} },
				{ L"mFuncEnter", 		  1, {CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_ENTER} },
				{ L"mFuncDelete",		  1, {CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_DELETE} },			
				{ L"mFuncCapsLock",		  1, {CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_CAPSLOCK} }
			};

			addKey(aKeyTable, 50);
		}
		else
		{
			tagKey aKeyTable[50] = 
			{
				{ L"mNum0", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_0} },
				{ L"mNum1", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_1} },
				{ L"mNum2", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_2} },
				{ L"mNum3", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_3} },
				{ L"mNum4", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_4} },
				{ L"mNum5", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_5} },
				{ L"mNum6", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_6} },
				{ L"mNum7", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_7} },
				{ L"mNum8", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_8} },
				{ L"mNum9", 1, {CKeyTable_ASC::KT_NUMBER}, {CKeyTable_ASC::NUM_9} },

				{ L"mLetA", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_A} },
				{ L"mLetB", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_B} },
				{ L"mLetC", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_C} },
				{ L"mLetD", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_D} },
				{ L"mLetE", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_E} },
				{ L"mLetF", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_F} },
				{ L"mLetG", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_G} },
				{ L"mLetH", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_H} },
				{ L"mLetI", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_I} },
				{ L"mLetJ", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_J} },
				{ L"mLetK", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_K} },
				{ L"mLetL", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_L} },
				{ L"mLetM", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_M} },
				{ L"mLetN", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_N} },
				{ L"mLetO", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_O} },
				{ L"mLetP", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_P} },
				{ L"mLetQ", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_Q} },
				{ L"mLetR", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_R} },
				{ L"mLetS", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_S} },
				{ L"mLetT", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_T} },
				{ L"mLetU", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_U} },
				{ L"mLetV", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_V} },
				{ L"mLetW", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_W} },
				{ L"mLetX", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_X} },
				{ L"mLetY", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_Y} },
				{ L"mLetZ", 1, {CKeyTable_ASC::KT_LETTER}, {CKeyTable_ASC::LET_Z} },

				{ L"mSignSpace",			 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_SPACE} },	
				{ L"mSignHash", 			 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_HASH} },
				{ L"mSignPercent",			 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_PERCENT} },
				{ L"mSignAnd", 				 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_AND} },
				{ L"mSignMultiplication",    1,	{CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_MULTIPLICATION} },
				{ L"mSignAddition", 		 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_ADDITION} },
				{ L"mSignSubtraction", 		 1,	{CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_SUBTRACTION} },
				{ L"mSignDot", 				 1,	{CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_DOT} },
				{ L"mSignDivision", 		 1, {CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_DIVISION} },
				{ L"mSignAt", 				 1,	{CKeyTable_ASC::KT_SIGN}, {CKeyTable_ASC::SIG_AT} },			

				{ L"mFuncBackspace",		 1, {CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_BACKSPACE} },
				{ L"mFuncEnter", 			 1,	{CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_ENTER} },
				{ L"mFuncDelete",			 1,	{CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_DELETE} },			
				{ L"mFuncCapsLock",			 1, {CKeyTable_ASC::KT_FUNC}, {CKeyTable_ASC::FUN_CAPSLOCK} }
			};

			addKey(aKeyTable, 50);
		}		

		return;
	}
}