#include "OpenHMI.h"

namespace OpenHMI
{
	ByteArray::ByteArray(UINT uiSize, UINT uiIncrement)
		: m_pbtBuffer(NULL)
		, m_uiBufferSize(0)
		, m_uiUsedSize(0)
		, m_uiIncrement(uiIncrement)
		, m_uiPosition(0)
	{
		setSize(uiSize);
	}

	ByteArray::~ByteArray() 
	{
		clear();
	}

	/*
	UINT ByteArray::write(const ByteArray &aryOther)
	{
		UINT uiSize = aryOther.bytesAvailable();
		writeBuffer(aryOther.m_pbtBuffer + aryOther.m_uiPosition, uiSize);
		return uiSize;
	}
	*/

	void ByteArray::writeBuffer(BYTE * pbtBuffer, UINT uiSize)
	{
		requstSize(m_uiPosition + uiSize);
		memcopy(m_pbtBuffer + m_uiPosition, pbtBuffer, uiSize);
		m_uiPosition += uiSize;
	}

	void ByteArray::writeBYTE(BYTE btVar)
	{
		writeBuffer(&btVar, 1);
	}

	void ByteArray::writeINT(INT iVar)
	{
		writeBuffer((BYTE*)&iVar, sizeof(INT));
	}

	void ByteArray::writeUINT(UINT uiVar)
	{
		writeBuffer((BYTE*)&uiVar, sizeof(UINT));
	}

	void ByteArray::writeSHORT(SHORT stVar)
	{
		writeBuffer((BYTE*)&stVar, sizeof(SHORT));
	}

	void ByteArray::writeUSHORT(USHORT ustVar)
	{
		writeBuffer((BYTE*)&ustVar, sizeof(USHORT));
	}

	void ByteArray::writeFLOAT(FLOAT fVar)
	{
		writeBuffer((BYTE*)&fVar, sizeof(FLOAT));
	}

	void ByteArray::writeDOUBLE(DOUBLE dVar)
	{
		writeBuffer((BYTE*)&dVar, sizeof(FLOAT));
	}

	/*
	UINT ByteArray::read(ByteArray &aryOther)
	{
		UINT uiReadSize = this->bytesAvailable();
		aryOther.writeBuffer(m_pbtBuffer + m_uiPosition, uiReadSize);
		m_uiPosition += uiReadSize;
		return uiReadSize;
	}
	*/

	UINT ByteArray::readBuffer(BYTE * pbtBuffer, UINT uiSize)
	{
		INT uiReadSize = uiSize;
		if ((m_uiPosition + uiSize) > m_uiUsedSize)
		{
			uiReadSize = m_uiUsedSize - m_uiPosition; // available Bytes
		}
		if (uiReadSize > 0)
		{
			memcopy(pbtBuffer, m_pbtBuffer + m_uiPosition, uiReadSize);
			m_uiPosition += uiReadSize;
		}
		return uiReadSize;
	}

	BYTE ByteArray::readBYTE()
	{
		BYTE btTemp = *(m_pbtBuffer + m_uiPosition);
		m_uiPosition += 1;
		return btTemp;
	}

	INT	ByteArray::readINT()
	{
		BYTE btTemp[sizeof(INT)];

		if (readBuffer(btTemp, sizeof(INT)) == sizeof(INT))
		{
			return *(INT*)(void*)btTemp;
		}
		return 0;
	}

	UINT ByteArray::readUINT()
	{
		BYTE btTemp[sizeof(UINT)];

		if (readBuffer(btTemp, sizeof(UINT)) == sizeof(UINT))
		{
			return *(UINT*)(void*)btTemp;
		}
		return 0;
	}

	SHORT ByteArray::readSHORT()
	{
		BYTE btTemp[sizeof(SHORT)];

		if (readBuffer(btTemp, sizeof(SHORT)) == sizeof(SHORT))
		{
			return *(SHORT*)(void*)btTemp;
		}
		return 0;		
	}

	USHORT ByteArray::readUSHORT()
	{
		BYTE btTemp[sizeof(USHORT)];

		if (readBuffer(btTemp, sizeof(USHORT)) == sizeof(USHORT))
		{
			return *(USHORT*)(void*)btTemp;
		}
		return 0;
	}

	FLOAT ByteArray::readFLOAT()
	{
		BYTE btTemp[sizeof(FLOAT)];

		if (readBuffer(btTemp, sizeof(FLOAT)) == sizeof(FLOAT))
		{
			return *(FLOAT*)(void*)btTemp;
		}
		return 0;
	}

	DOUBLE	ByteArray::readDOUBLE()
	{
		BYTE btTemp[sizeof(DOUBLE)];

		if (readBuffer(btTemp, sizeof(DOUBLE)) == sizeof(DOUBLE))
		{
			return *(DOUBLE*)(void*)btTemp;
		}
		return 0;
	}

	UINT ByteArray::getSize() const
	{
		return m_uiUsedSize;
	}

	void ByteArray::setSize(UINT uiSize)
	{
		// In fact, we can retrieve the wasted memory here. #TODO
		if (uiSize > 0)
		{
			requstSize(uiSize);
		}
		else
		{
			clear();
		}
	}

	void ByteArray::setIncrement(UINT uiIncrement)
	{
		m_uiIncrement = uiIncrement;
	}

	UINT ByteArray::getPosition() const
	{
		return m_uiPosition;
	}

	void ByteArray::setPosition(UINT uiPosition)
	{
		UINT uiMaxPos = m_uiUsedSize;
		m_uiPosition = uiPosition <= uiMaxPos ? uiPosition : uiMaxPos;
	}
	
	void ByteArray::clear()
	{
		if (m_pbtBuffer != NULL)
		{
			delete[] m_pbtBuffer;
			m_pbtBuffer = NULL;
		}
		
		m_uiBufferSize = 0;
		m_uiUsedSize = 0;
		m_uiPosition = 0;
	}

	UINT ByteArray::bytesAvailable() const
	{
		return m_uiUsedSize - m_uiPosition;
	}

	void ByteArray::requstSize(UINT uiSize)
	{
		if (m_uiBufferSize >= uiSize)
		{
			m_uiUsedSize = uiSize;
			return;
		}

		// Size set to multiple UINT, and strategy may be adjusted here.
		UINT uiNewBufferSize = uiSize /*+ uiReqestSize % sizeof(UINT)*/ + m_uiIncrement;
		BYTE *pbtTemp = new BYTE[uiNewBufferSize]; // We shall call relloc func in the future.
		memclear(pbtTemp, 0, uiNewBufferSize);

		if (pbtTemp != NULL)
		{
			// Move data to new buffer
			if (m_pbtBuffer != NULL)
			{
				memcopy(pbtTemp, m_pbtBuffer, m_uiUsedSize);
				delete[] m_pbtBuffer;
			}
			m_pbtBuffer = pbtTemp;			
			m_uiBufferSize = uiNewBufferSize;
			m_uiUsedSize = uiSize;
		}
		else
		{
			// Memory alloc failed, how to do?
		}
	}

	void ByteArray::memcopy(BYTE * pbtDst, BYTE * pbtSrc, UINT uiSize)
	{
		UINT uiCount = 0;
		while (uiCount++ < uiSize)
		{
			*pbtDst++ = *pbtSrc++;
		}
	}

	void ByteArray::memclear(BYTE * pbtDst, INT iVar, UINT uiSize)
	{
		UINT uiCount = 0;
		while (uiCount++ < uiSize)
		{
			*pbtDst++ = iVar;
		}
	}
}