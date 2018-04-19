/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	List2	
	*			It is a common list class and its member must be an instance of class T.
	*			The type T must support "=" operation.
	*/
	template <typename T>
	class List2 : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagNode
		{
			T		data;
			tagNode	*pPre;
			tagNode	*pNext;

			tagNode(T tData) 
				: data(tData)
				, pPre(NULL)
				, pNext(NULL)
			{
				;
			}
		};

	public:// method
		List2() 
			: m_pFront(NULL)
			, m_pRear(NULL)
			, m_pCur(NULL)
			, m_ulSize(0)
		{
			;
		}

		virtual ~List2(void)
		{
			clear();
		}

		/**
		* @brief	Begin to access to each item of the list2.
		*			Used with hasMore and next functions together.
		*			Example:
		*
		*			List2<T> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ T &t = temp.next(); }
		*			
		* @return	void
		*/
		void resetIterator()
		{
			m_pCur = m_pFront;
		}

		BOOLEAN hasMore()
		{
			return m_pCur != NULL ? TRUE : FALSE;
		}

		T& next()
		{
			if (m_pCur != NULL)
			{
				tagNode *pNode = m_pCur;
				m_pCur = m_pCur->pNext;
				return pNode->data;
			}

			return m_tTemp;
		}

		BOOLEAN isExist(const T &data) const
		{
			tagNode *pNode = m_pFront;
			while (pNode != NULL)
			{
				if (pNode->data == data)
				{
					return TRUE;
				}
				pNode = pNode->pNext;
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
				return m_tTemp;
			}

			tagNode *pNode = m_pFront;
			for (UINT i = 0; i < uiIndex; i++)
			{
				pNode = pNode->pNext;
			}

			return pNode->data;
		}

		T operator [](UINT uiIndex) const
		{
			if (uiIndex >= m_ulSize)
			{
				return m_tTemp;
			}

			tagNode *pNode = m_pFront;
			for (UINT i = 0; i < uiIndex; i++)
			{
				pNode = pNode->pNext;
			}

			return pNode->data;
		}

		LONG getItemIndex(const T &data) const
		{
			tagNode *pNode = m_pFront;
			for (LONG i = 0; i < m_ulSize; i++)
			{
				if (pNode->data == data)
				{
					return i;
				}
				pNode = pNode->pNext;
			}

			return -1;
		}		

		void addFirst(const T &data)
		{
			tagNode *pNode = new tagNode(data);
			if (pNode != NULL)
			{
				pNode->pNext = m_pFront;

				if (m_pFront != NULL)
				{
					m_pFront->pPre = pNode;
				}
				m_pFront = pNode;
				m_ulSize ++;

				if (m_ulSize == 1)
				{
					m_pRear = m_pFront;
					m_pCur = m_pFront;
				}
			}
		}

		void addLast(const T &data)
		{
			tagNode *pNode = new tagNode(data);
			if (pNode != NULL)
			{
				pNode->pPre = m_pRear;

				if (m_pRear != NULL)
				{
					m_pRear->pNext = pNode;
				}
				m_pRear = pNode;
				m_ulSize ++;

				if (m_ulSize == 1)
				{
					m_pFront = m_pRear;
					m_pCur = m_pRear;
				}
			}
		}

		void insert(ULONG ulIndex, const T &data)
		{
			if (ulIndex = 0)
			{
				addFirst(data);
				return;
			}
			else if (ulIndex >= m_ulSize)
			{
				addLast(data);
				return;
			}
			else
			{
				tagNode *pNewNode = new tagNode(data);
				if (pNewNode != NULL)
				{
					tagNode *pNode = m_pFront;
					for (ULONG i = 0; i < ulIndex; i++)
					{
						pNode = pNode->pNext;
					}

					pNewNode->pPre = pNode->pPre;
					pNode->pPre->pNext = pNewNode;
					pNode->pPre = pNewNode;
					pNewNode->pNext = pNode;

					m_ulSize ++;
				}
			}
		}

		BOOLEAN deleteItem(const T &data)
		{
			if (m_ulSize == 0)
			{
				// do nothing
			}
			else if (m_ulSize == 1)
			{
				if (data == m_pFront->data)
				{
					deleteNode(m_pFront);

					m_pFront = m_pRear = m_pCur = NULL;
					m_ulSize = 0;

					return TRUE;
				}
			}
			else
			{
				if (data == m_pFront->data)
				{
					tagNode *pNode = m_pFront;

					m_pFront = m_pFront->pNext;
					m_pFront->pPre = NULL;

					deleteNode(pNode);
					m_ulSize--;

					return TRUE;
				}
				else if (data == m_pRear->data)
				{
					tagNode *pNode = m_pRear;

					m_pRear = m_pRear->pPre;
					m_pRear->pNext = NULL;

					deleteNode(pNode);
					m_ulSize--;

					return TRUE;
				}
				else
				{
					tagNode *pNode = m_pFront;
					while (pNode != NULL)
					{
						if (pNode->data == data)
						{
							pNode->pNext->pPre = pNode->pPre;
							pNode->pPre->pNext = pNode->pNext;

							deleteNode(pNode);
							m_ulSize --;

							return TRUE;
						}

						pNode = pNode->pNext;
					}
				}
			}			
			
			return FALSE;
		}

		BOOLEAN deleteItemByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return FALSE;
			}

			if (m_ulSize == 1)
			{
				deleteNode(m_pFront);

				m_pFront = m_pRear = m_pCur = NULL;
				m_ulSize = 0;

				return TRUE;
			}
			else
			{
				if (uiIndex == 0)
				{
					tagNode *pNode = m_pFront;

					m_pFront = m_pFront->pNext;
					m_pFront->pPre = NULL;

					deleteNode(pNode);
					m_ulSize--;

					return TRUE;
				}
				else if (uiIndex == m_ulSize - 1)
				{
					tagNode *pNode = m_pRear;

					m_pRear = m_pRear->pPre;
					m_pRear->pNext = NULL;

					deleteNode(pNode);
					m_ulSize--;

					return TRUE;
				}
				else
				{
					tagNode *pNode = m_pFront;
					for (UINT i = 0; i < uiIndex; i++)
					{
						pNode = pNode->pNext;
					}

					pNode->pNext->pPre = pNode->pPre;
					pNode->pPre->pNext = pNode->pNext;

					deleteNode(pNode);
					m_ulSize --;

					return TRUE;
				}
			}

			return FALSE;
		}

		T popFirst()
		{
			if (m_ulSize == 0)
			{
				return NULL;
			}
			else if (m_ulSize == 1)
			{
				T data = m_pFront->data;

				delete m_pFront;

				m_pFront = m_pRear = m_pCur = NULL;
				m_ulSize = 0;

				return data;
			}
			else
			{
				tagNode *pNode = m_pFront;

				m_pFront = m_pFront->pNext;
				m_pFront->pPre = NULL;

				T data = pNode->data;

				delete pNode;
				m_ulSize--;

				return data;
			}
		}

		T popLast()
		{
			if (m_ulSize == 0)
			{
				return NULL;
			}
			else if (m_ulSize == 1)
			{
				T data = m_pRear->data;

				delete m_pRear;

				m_pFront = m_pRear = m_pCur = NULL;
				m_ulSize = 0;

				return data;
			}
			else
			{
				tagNode *pNode = m_pRear;

				m_pRear = m_pRear->pPre;
				m_pRear->pNext = NULL;

				T data = pNode->data;

				delete pNode;
				m_ulSize--;

				return data;
			}
		}

		T* getFirst()
		{
			if (m_pFront != NULL)
			{
				return m_pFront->data;
			}
			else
			{
				return NULL;
			}
		}

		T* getLast()
		{
			if (m_pRear != NULL)
			{
				return m_pRear->data;
			}
			else
			{
				return NULL;
			}
		}

		void clear()
		{
			tagNode *pNode = NULL;

			while (m_pFront != NULL)
			{
				pNode = m_pFront;
				m_pFront = m_pFront->pNext;

				deleteNode(pNode);
			}

			m_pFront = m_pRear = m_pCur = NULL;
			m_ulSize = 0;
		}

	protected:// method
		List2(const List2<T>&){}
		List2<T>& operator =(const List2<T>&){return *this;}

	private:// method
		void deleteNode(tagNode *pNode)
		{
			delete pNode;
		}

	protected:// property

	private:// property
		tagNode		*m_pFront;
		tagNode		*m_pRear;
		tagNode		*m_pCur;
		ULONG		m_ulSize;
		T			m_tTemp;
	};
}