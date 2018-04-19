/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	List	
	*			It is a common list class and its member must be a pointer of class T.
	*			By default, the list takes charge of its member's lifecycle (Automatically free when remove from this list.).
	*			What's more, you can cancel this behavior via setting parameter "bFullControl" to FALSE.
	*/
	template <typename T>
	class List : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagNode
		{
			T		*pData;
			tagNode	*pPre;
			tagNode	*pNext;

			tagNode(T *pTData) 
				: pData(pTData)
				, pPre(NULL)
				, pNext(NULL)
			{
				;
			}
		};

	public:// method
		/**
		* @brief	The constructor of the class.
		*
		* @param	bFullControl Used to set the property that manage its member's lifecycle or not.
		*/
		List(BOOLEAN bFullControl = TRUE) 
			: m_bFullControl(bFullControl)
			, m_pFront(NULL)
			, m_pRear(NULL)
			, m_pCur(NULL)
			, m_ulSize(0)
		{
			;
		}

		virtual ~List(void)
		{
			clear();
		}

		/**
		* @brief	Begin to access to each item of the list.
		*			Used with hasMore and next functions together.
		*			Example:
		*
		*			List<T> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ T *pT = temp.next(); }
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

		T* next()
		{
			if (m_pCur != NULL)
			{
				tagNode *pNode = m_pCur;
				m_pCur = m_pCur->pNext;
				return pNode->pData;
			}

			return NULL;
		}

		BOOLEAN isExist(T *pData) const
		{
			tagNode *pNode = m_pFront;
			while (pNode != NULL)
			{
				if (pNode->pData == pData)
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

		T* operator [](UINT uiIndex) const
		{
			if (uiIndex >= m_ulSize)
			{
				return NULL;
			}

			tagNode *pNode = m_pFront;
			for (UINT i = 0; i < uiIndex; i++)
			{
				pNode = pNode->pNext;
			}

			return pNode->pData;
		}

		LONG getItemIndex(T *pData) const
		{
			tagNode *pNode = m_pFront;
			for (LONG i = 0; i < m_ulSize; i++)
			{
				if (pNode->pData == pData)
				{
					return i;
				}
				pNode = pNode->pNext;
			}

			return -1;
		}		

		void addFirst(T *pData)
		{
			tagNode *pNode = new tagNode(pData);
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

		void addLast(T *pData)
		{
			tagNode *pNode = new tagNode(pData);
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

		void insert(ULONG ulIndex, T *pData)
		{
			if (ulIndex = 0)
			{
				addFirst(pData);
				return;
			}
			else if (ulIndex >= m_ulSize)
			{
				addLast(pData);
				return;
			}
			else
			{
				tagNode *pNewNode = new tagNode(pData);
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

		BOOLEAN deleteItem(T *pData)
		{
			if (m_ulSize == 0)
			{
				// do nothing
			}
			else if (m_ulSize == 1)
			{
				if (pData == m_pFront->pData)
				{
					deleteNode(m_pFront);

					m_pFront = m_pRear = m_pCur = NULL;
					m_ulSize = 0;

					return TRUE;
				}
			}
			else
			{
				if (pData == m_pFront->pData)
				{
					tagNode *pNode = m_pFront;

					m_pFront = m_pFront->pNext;
					m_pFront->pPre = NULL;

					deleteNode(pNode);
					m_ulSize--;

					return TRUE;
				}
				else if (pData == m_pRear->pData)
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
						if (pNode->pData == pData)
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

		T* popFirst()
		{
			if (m_ulSize == 0)
			{
				return NULL;
			}
			else if (m_ulSize == 1)
			{
				T *pData = m_pFront->pData;

				delete m_pFront;

				m_pFront = m_pRear = m_pCur = NULL;
				m_ulSize = 0;

				return pData;
			}
			else
			{
				tagNode *pNode = m_pFront;

				m_pFront = m_pFront->pNext;
				m_pFront->pPre = NULL;

				T *pData = pNode->pData;

				delete pNode;
				m_ulSize--;

				return pData;
			}
		}

		T* popLast()
		{
			if (m_ulSize == 0)
			{
				return NULL;
			}
			else if (m_ulSize == 1)
			{
				T *pData = m_pRear->pData;

				delete m_pRear;

				m_pFront = m_pRear = m_pCur = NULL;
				m_ulSize = 0;

				return pData;
			}
			else
			{
				tagNode *pNode = m_pRear;

				m_pRear = m_pRear->pPre;
				m_pRear->pNext = NULL;

				T *pData = pNode->pData;

				delete pNode;
				m_ulSize--;

				return pData;
			}
		}

		T* getFirst()
		{
			if (m_pFront != NULL)
			{
				return m_pFront->pData;
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
				return m_pRear->pData;
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
		List(const List<T>&){}
		List<T>& operator =(const List<T>&){return *this;}

	private:// method
		void deleteNode(tagNode *pNode)
		{
			if (m_bFullControl)
			{
				if (pNode->pData != NULL)
				{
					delete pNode->pData;
				}
			}

			delete pNode;
		}

	protected:// property

	private:// property
		BOOLEAN		m_bFullControl;
		tagNode		*m_pFront;
		tagNode		*m_pRear;
		tagNode		*m_pCur;
		ULONG		m_ulSize;
	};
}