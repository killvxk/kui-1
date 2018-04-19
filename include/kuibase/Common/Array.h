/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	Array	
	*			It is a common array class and its member must be a pointer of class T.
	*			By default, the array takes charge of its member's life cycle (Automatically free when remove from this array.).
	*			What's more, you can cancel this behavior via setting parameter "bFullControl" to FALSE.
	*/
	template <typename T>
	class Array : public Object
	{
	public:// const define

	private:// const define
		static const ULONG INITIAL_CAPACITY = 10;

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		/**
		* @brief	The constructor of the class.
		*
		* @param	bFullControl Used to set the property that manage its member's life cycle or not.
		* @param	ulCapacity Initialize the pool size first.
		*/
		Array(BOOLEAN bFullControl = TRUE, ULONG ulCapacity = INITIAL_CAPACITY) 
			: m_bFullControl(bFullControl)
			, m_ulCapacity(ulCapacity)
			, m_ulSize(0)
			, m_ulPosition(0)
		{ 
			if (m_ulCapacity < INITIAL_CAPACITY)
			{
				m_ulCapacity = INITIAL_CAPACITY;
			}

			m_ppArray = new T* [m_ulCapacity];

			for (ULONG i = 0; i < m_ulCapacity; i++)
			{
				m_ppArray[i] = NULL;
			}
		}

		virtual ~Array(void)
		{
			clear();
		}

		/**
		* @brief	Begin to access to each item of the array.
		*			Used with hasMore and next functions together.
		*			Example:
		*
		*			Array<T> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ T *pT = temp.next(); }
		*			
		* @return	void
		*/
		void resetIterator()
		{
			m_ulPosition = 0;
		}

		BOOLEAN hasMore()
		{
			return m_ulPosition < m_ulSize ? TRUE : FALSE;
		}

		T* next()
		{
			if (m_ulPosition < m_ulSize)
			{
				T *pData = m_ppArray[m_ulPosition++];
				return pData;
			}		

			return NULL;
		}		

		BOOLEAN isExist(T *pData) const
		{
			for (ULONG i = 0; i < m_ulSize; i++)
			{
				if (m_ppArray[i] == pData)
				{
					return TRUE;
				}
			}

			return FALSE;
		}

		BOOLEAN isEmpty(void) const
		{
			return m_ulSize == 0 ? TRUE : FALSE;
		}

		ULONG size(void) const
		{
			return m_ulSize;
		}

		T* operator [](UINT uiIndex) const
		{
			if (uiIndex >= m_ulSize)
			{
				return NULL;
			}

			return m_ppArray[uiIndex];
		}

		T* getItem(UINT uiIndex) const
		{
			if (uiIndex >= m_ulSize)
			{
				return NULL;
			}

			return m_ppArray[uiIndex]; 
		}

		LONG getItemIndex(T *pData) const
		{
			for (ULONG i = 0; i < m_ulSize; i++)
			{
				if (m_ppArray[i] == pData)
				{
					return i;
				}
			}

			return -1;
		}

		void addItem(T *pData)
		{
			if (ensureCapacity(m_ulSize + 1))
			{
				m_ppArray[m_ulSize++] = pData;
			}
		}

		void addItems(const Array<T>& src)
		{
			if (src.size() == 0)
			{
				return;
			}

			if (ensureCapacity(m_ulSize + src.size()))
			{
				for( ULONG i = 0; i < src.size(); i++)
				{
					m_ppArray[m_ulSize++] = src[i];
				}

				if (m_bFullControl && src.m_bFullControl)
				{
					delete [] src.m_ppArray;
					src.m_ppArray = NULL;
					src.m_ulCapacity = 0;
					src.m_ulSize = 0;
					src.m_ulPosition = 0;
				}
			}
		}

		void insertItem(ULONG ulIndex, T *pData)
		{
			if (ulIndex >= m_ulSize)
			{
				addItem(pData);
				return;
			}

			if (ensureCapacity(m_ulSize + 1))
			{
				for (LONG i = LONG(m_ulSize - 1); i >= (LONG)ulIndex; i--)
				{
					m_ppArray[i + 1] = m_ppArray[i];
				}

				m_ppArray[ulIndex] = pData;
				m_ulSize++;
			}
		}

		void insertItems(ULONG ulIndex, const Array<T>& src)
		{
			if (src.size() == 0)
			{
				return;
			}

			if (ulIndex >= m_ulSize)
			{
				addItems(src);
				return;
			}

			ULONG ulSrcSize = src.size();
			if (ensureCapacity(m_ulSize + ulSrcSize))
			{
				for (LONG i = LONG(m_ulSize - 1); i >= (LONG)ulIndex; i--)
				{
					m_ppArray[i + ulSrcSize] = m_ppArray[i];
				}

				for( ULONG j = 0; j < ulSrcSize; j++)
				{
					m_ppArray[ulIndex + j] = src[j];
				}

				m_ulSize += ulSrcSize;

				if (m_bFullControl && src.m_bFullControl)
				{
					delete [] src.m_ppArray;
					src.m_ppArray = NULL;
					src.m_ulCapacity = 0;
					src.m_ulSize = 0;
					src.m_ulPosition = 0;
				}
			}
		}

		BOOLEAN popItem(T *pData)
		{
			UINT uiIndex = getItemIndex(pData);

			if (uiIndex >= m_ulSize)
			{
				return FALSE;
			}

			m_ppArray[uiIndex] = NULL;

			arrayCopy(m_ppArray + uiIndex, m_ppArray + (uiIndex + 1), m_ulSize - uiIndex - 1);				
			m_ppArray[m_ulSize - 1] = NULL;
			m_ulSize --;

			return TRUE;
		}

		T* popItemByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return NULL;
			}

			T* pTemp = m_ppArray[uiIndex];
			m_ppArray[uiIndex] = NULL;

			arrayCopy(m_ppArray + uiIndex, m_ppArray + (uiIndex + 1), m_ulSize - uiIndex - 1);				
			m_ppArray[m_ulSize - 1] = NULL;
			m_ulSize --;

			return pTemp;
		}

		BOOLEAN deleteItem(T *pData)
		{
			return deleteItemByIndex(getItemIndex(pData));
		}		

		BOOLEAN deleteItemByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return FALSE;
			}

			if (m_bFullControl)
			{
				if (m_ppArray[uiIndex] != NULL)
				{
					delete m_ppArray[uiIndex];					
				}
			}
			m_ppArray[uiIndex] = NULL;

			arrayCopy(m_ppArray + uiIndex, m_ppArray + (uiIndex + 1), m_ulSize - uiIndex - 1);				
			m_ppArray[m_ulSize - 1] = NULL;
			m_ulSize --;		

			return TRUE;
		}

		template<typename TCompare>
		void sort(TCompare compare)
		{
			if (m_ulSize <= 1)
			{
				return;
			}

			for (ULONG i = 0; i < m_ulSize - 1; i++)
			{
				for (ULONG j = 0; j < m_ulSize - i - 1; j++)
				{
					if (compare(m_ppArray[j], m_ppArray[j + 1]) > 0)
					{
						T tTemp = m_ppArray[j + 1];
						m_ppArray[j + 1] = m_ppArray[j];
						m_ppArray[j] = tTemp;
					}
				}
			}
		}

		void clear()
		{
			if (m_ppArray != NULL)
			{
				if (m_bFullControl)
				{
					for (ULONG i = 0; i < m_ulSize; i++)
					{
						if (m_ppArray[i] != NULL)
						{
							delete m_ppArray[i];
						}
					}
				}

				delete [] m_ppArray;
				m_ppArray = NULL;
				m_ulCapacity = 0;
			}

			m_ulSize = 0;
			m_ulPosition = 0;
		}

	protected:// method
		Array(const Array<T>&){}
		Array<T>& operator =(const Array<T>&){return *this;}

	private:// method
		BOOLEAN ensureCapacity(ULONG ulMinCapacity)
		{
			if (m_ulCapacity >= ulMinCapacity)
			{
				return TRUE;
			}

			while (m_ulCapacity < ulMinCapacity)
			{
				m_ulCapacity = m_ulCapacity * 3 / 2 + 1;
			}
			
			T **ppNewArray = new T* [m_ulCapacity];
			if (ppNewArray != NULL)
			{
				for (ULONG i = 0; i < m_ulCapacity; i++)
				{
					ppNewArray[i] = NULL;
				}

				arrayCopy(ppNewArray, m_ppArray, m_ulSize);

				if (m_ppArray != NULL)
				{
					delete [] m_ppArray;
				}
				m_ppArray = ppNewArray;

				return TRUE;
			}

			return FALSE;
		}

		void arrayCopy(T **pDest, T **pOrig, ULONG ulSize)
		{
			if (pDest != NULL && pOrig != NULL && ulSize > 0)
			{
				T **pTail = pOrig + ulSize;
				while (pOrig < pTail)
				{
					*pDest++ = *pOrig++;
				}
			}
		}

	protected:// property	

	private:// property
		BOOLEAN		m_bFullControl;
		T			**m_ppArray;
		ULONG		m_ulCapacity;
		ULONG		m_ulSize;
		ULONG		m_ulPosition;
	};
}