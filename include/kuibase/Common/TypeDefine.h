/*@Author Zeki.Yan*/
#pragma once

namespace OpenHMI
{
	typedef	unsigned	char	BYTE;
	typedef				wchar_t	CHAR;
	typedef				short	SHORT;
	typedef				int		INT;
	typedef				long	LONG;	
	typedef	unsigned	short	USHORT;
	typedef	unsigned	int		UINT;
	typedef	unsigned	long	ULONG;
	typedef				float	FLOAT;
	typedef				double	DOUBLE;
	typedef				bool	BOOLEAN;

	typedef				BYTE*	PBYTE;
	typedef				CHAR*	PCHAR;
	typedef				SHORT*	PSHORT;
	typedef				INT*	PINT;
	typedef				LONG*	PLONG;
	typedef				USHORT*	PUSHORT;
	typedef				UINT*	PUINT;
	typedef				ULONG*	PULONG;
	typedef				FLOAT*	PFLOAT;
	typedef				DOUBLE*	PDOULBE;
	typedef				BOOLEAN*PBOOLEAN;

	typedef				void(*PCALLBACK)(Object *pThis);
	typedef				void(*PCALLBACK2)(Object *pThis, Object *pParam);

	#define NULL    0
}