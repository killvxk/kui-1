/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	Array2	
	*			It is a common array class and its member must be an instance of class T.
	*			The type T must support "=" operation.
	*/
	template <typename T>
	class Array2 : public Object
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
		* @param	ulCapacity Initialize the pool size first.
		*/
		Array2(ULONG ulCapacity = INITIAL_CAPACITY) 
			: m_ulCapacity(ulCapacity)
			, m_ulSize(0)
			, m_ulPosition(0)
		{ 
			if (m_ulCapacity < INITIAL_CAPACITY)
			{
				m_ulCapacity = INITIAL_CAPACITY;
			}

			m_pArray = new T[m_ulCapacity];
		}

		virtual ~Array2(void)
		{
			clear();
		}

		void resize(UINT uiSize)
		{
			ensureCapacity(uiSize);
			m_ulSize = uiSize;
		}

		/**
		* @brief	Begin to access to each item of the array2.
		*			Used with hasMore and next functions together.
		*			Example:
		*
		*			Array2<T> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ T &t = temp.next(); }
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

		T& next()
		{
			if (m_ulPosition < m_ulSize)
			{
				return m_pArray[m_ulPosition++];
			}

			return m_tValue;
		}		

		BOOLEAN isExist(const T &data) const
		{
			for (ULONG i = 0; i < m_ulSize; i++)
			{
				if (m_pArray[i] == data)
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

		T& operator [](UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return m_tValue;
			}

			return m_pArray[uiIndex];
		}

		T operator [](UINT uiIndex) const
		{
			if (uiIndex >= m_ulSize)
			{
				return m_tValue;
			}

			return m_pArray[uiIndex];
		}

		LONG getItemIndex(const T &data) const
		{
			for (ULONG i = 0; i < m_ulSize; i++)
			{
				if (m_pArray[i] == data)
				{
					return i;
				}
			}

			return -1;
		}

		void addItem(const T &data)
		{
			if (ensureCapacity(m_ulSize + 1))
			{
				m_pArray[m_ulSize++] = data;
			}
		}		
		
		void addItems(const Array2<T>& src)
		{
			if (src.size() == 0)
			{
				return;
			}

			if (ensureCapacity(m_ulSize + src.size()))
			{
				for( ULONG i = 0; i < src.size(); i++)
				{
					m_pArray[m_ulSize++] = src[i];
				}
			}
		}

		void addItems(UINT uiCount, const T &data)
		{
			if (uiCount == 0)
			{
				return;
			}

			if (ensureCapacity(m_ulSize + uiCount))
			{
				for( ULONG i = 0; i < uiCount; i++)
				{
					m_pArray[m_ulSize++] = data;
				}
			}
		}

		void insertItem(ULONG ulIndex, const T &data)
		{
			if (ulIndex >= m_ulSize)
			{
				addItem(data);
				return;
			}

			if (ensureCapacity(m_ulSize + 1))
			{
				for (LONG i = LONG(m_ulSize - 1); i >= (LONG)ulIndex; i--)
				{
					m_pArray[i + 1] = m_pArray[i];
				}

				m_pArray[ulIndex] = data;
				m_ulSize++;
			}
		}

		void insertItems(ULONG ulIndex, const Array2<T>& src)
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
					m_pArray[i + ulSrcSize] = m_pArray[i];
				}

				for( ULONG j = 0; j < ulSrcSize; j++)
				{
					m_pArray[ulIndex + j] = src[j];
				}

				m_ulSize += ulSrcSize;
			}
		}

		void insertItems(ULONG ulIndex, UINT uiCount, const T &data)
		{
			if (uiCount == 0)
			{
				return;
			}

			if (ulIndex >= m_ulSize)
			{
				addItems(uiCount, data);
				return;
			}

			if (ensureCapacity(m_ulSize + uiCount))
			{
				for (LONG i = LONG(m_ulSize - 1); i >= (LONG)ulIndex; i--)
				{
					m_pArray[i + uiCount] = m_pArray[i];
				}

				for( ULONG j = 0; j < uiCount; j++)
				{
					m_pArray[ulIndex + j] = data;
				}

				m_ulSize += uiCount;
			}
		}

		BOOLEAN deleteItem(const T &data)
		{
			return deleteItemByIndex(getItemIndex(data));
		}

		BOOLEAN deleteItemByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return FALSE;
			}

			arrayCopy(m_pArray + uiIndex, m_pArray + (uiIndex + 1), m_ulSize - uiIndex - 1);
			m_ulSize--;

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
					if (compare(m_pArray[j], m_pArray[j + 1]) > 0)
					{
						T tTemp = m_pArray[j + 1];
						m_pArray[j + 1] = m_pArray[j];
						m_pArray[j] = tTemp;
					}
				}
			}
		}

		void clear()
		{
			if (m_pArray != NULL)
			{
				delete [] m_pArray;
				m_pArray = NULL;
				m_ulCapacity = 0;
			}

			m_ulSize = 0;
			m_ulPosition = 0;
		}	

	protected:// method
		Array2(const Array2<T>&){}
		Array2<T>& operator =(const Array2<T>&){return *this;}

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
			
			T *pNewArray = new T[m_ulCapacity];
			if (pNewArray != NULL)
			{
				arrayCopy(pNewArray, m_pArray, m_ulSize);

				if (m_pArray != NULL)
				{
					delete [] m_pArray;
				}
				m_pArray = pNewArray;

				return TRUE;
			}

			return FALSE;
		}

		void arrayCopy(T *pDest, T *pOrig, ULONG ulSize)
		{
			if (pDest != NULL && pOrig != NULL && ulSize > 0)
			{
				T *pTail = pOrig + ulSize;
				while (pOrig < pTail)
				{
					*pDest++ = *pOrig++;
				}
			}
		}

	protected:// property	

	private:// property
		T			*m_pArray;
		ULONG		m_ulCapacity;
		ULONG		m_ulSize;
		ULONG		m_ulPosition;
		T			m_tValue;
	};
}