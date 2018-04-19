/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	ByteArray	
	*			It is a special array for BYTE type.
	*/
	class OPENHMI_API ByteArray : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		ByteArray(UINT uiSize = 0, UINT uiIncrement = 1024);
		virtual ~ByteArray();

		//UINT	write(const ByteArray &aryOther);
		void	writeBuffer(BYTE * pbtBuffer, UINT uiSize);
		void 	writeBYTE(BYTE btVar);
		void 	writeINT(INT iVar);
		void	writeUINT(UINT uiVar);
		void	writeSHORT(SHORT stVar);
		void	writeUSHORT(USHORT ustVar);
		void	writeFLOAT(FLOAT fVar);
		void	writeDOUBLE(DOUBLE dVar);

		//UINT	read(ByteArray &aryOther);
		UINT	readBuffer(BYTE * pbtBuffer, UINT uiSize);
		BYTE	readBYTE();
		INT		readINT();
		UINT 	readUINT();
		SHORT	readSHORT();
		USHORT	readUSHORT();
		FLOAT	readFLOAT();
		DOUBLE	readDOUBLE();

		UINT	getSize() const;
		void	setSize(UINT uiSize);
		void	setIncrement(UINT uiIncrement);
		UINT 	getPosition() const;
		void	setPosition(UINT uiPosition);

		void	clear();
		UINT	bytesAvailable() const;

		BYTE*	buffer() const { return m_pbtBuffer; };

	protected:// method
		ByteArray(const ByteArray&){}
		ByteArray& operator =(const ByteArray&){return *this;}

	private:// method
		void 	requstSize(UINT uiSize);
		void	memcopy(BYTE * pbtDst, BYTE * pbtSrc, UINT uiSize);
		void	memclear(BYTE * pbtDst, INT iVar, UINT uiSize);

	protected:// property

	private:// property
		BYTE*	m_pbtBuffer;
		UINT	m_uiBufferSize;
		UINT	m_uiUsedSize;
		UINT	m_uiIncrement;		
		UINT	m_uiPosition;
	};
}