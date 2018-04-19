/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	Map	
	*			It is a common map class and its value part member must be a pointer of class TValue.
	*			By default, the map takes charge of its value part member's lifecycle (Automatically free when remove from this map.).
	*			What's more, you can cancel this behavior via setting parameter "bFullControl" to FALSE.
	*			The type TKey must support "=" operation.
	*			Generally you can use tagNumCompare or tagStringCompare as TKey comparing.
	*/
	template<typename TKey,typename TValue, typename TCompare>
	class Map : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum		

	private:// embed class or struct or enum
		struct tagPair
		{
			TKey	*pKey;
			TValue	*pData;

			tagPair(TKey *pTKey, TValue *pTData) 
				: pKey(pTKey)
				, pData(pTData)
			{
				;
			}	

			tagPair() : pKey(NULL), pData(NULL)
			{

			}
		};

		struct tagNode
		{
			TKey	key;
			TValue	*pData;
			tagNode *pLeft;
			tagNode *pRight;
			tagNode *pParent;

			tagNode(const TKey &tkey, TValue *pTData)
				: key(tkey)
				, pData(pTData)
				, pLeft(NULL)
				, pRight(NULL)
				, pParent(NULL)
			{
				;
			}
		};

	public:// method
		/**
		* @brief	The constructor of the class.
		*
		* @param	bFullControl Used to set the property that manage its value part member's lifecycle or not.
		*/
		Map(BOOLEAN bFullControl = TRUE)
			: m_bFullControl(bFullControl)
			, m_pHeadNode(NULL)
			, m_pIterator(NULL)
			, m_ulSize(0)			
			, m_ulItemCur(0)
			, m_ulIteratorCur(0)
		{
			;
		}

		virtual ~Map()
		{
			clear();
		}

		/**
		* @brief	Begin to access to each item of the map.
		*			Used with hasMore and next (nextKey, nextKeyAndValue) functions together.
		*			Example:
		*
		*			Map<TKey, TValue, TCompare> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ TValue *pTValue = temp.next(); }
		*			
		* @return	void
		*/
		void resetIterator()
		{
			clearIterator();

			if ( isEmpty() )
			{
				return;
			}

			m_pIterator = new tagPair[m_ulSize];
			createIteratorByLDR(m_pHeadNode);
		}

		BOOLEAN hasMore()
		{
			return (m_ulItemCur < m_ulSize) ? TRUE : FALSE;
		}

		TValue* next()
		{
			return m_pIterator[m_ulItemCur++].pData;
		}

		TKey nextKey()
		{
			return *(m_pIterator[m_ulItemCur++].pKey);
		}

		MapPair<TKey, TValue> nextKeyAndValue()
		{
			MapPair<TKey, TValue> pair(m_pIterator[m_ulItemCur].pKey, m_pIterator[m_ulItemCur].pData);

			m_ulItemCur++;

			return pair;
		}
		
		BOOLEAN isExist(const TKey &key) const
		{
			tagNode* node = isItemExist(m_pHeadNode, key);

			return (node == NULL) ? FALSE : TRUE;
		}

		BOOLEAN isEmpty(void) const
		{
			return (m_ulSize > 0) ? FALSE : TRUE;
		}

		ULONG size(void) const
		{
			return m_ulSize;
		}

		TValue* operator[](const TKey &key) const
		{
			return getValueByKey(key);
		}

		TValue* getValueByKey(const TKey &key) const
		{
			tagNode* pNode = isItemExist(m_pHeadNode, key);

			return pNode == NULL ? NULL : pNode->pData;
		}

		TValue* getValueByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return NULL;
			}

			if (m_pIterator == NULL)
			{
				resetIterator();
			}

			return m_pIterator[uiIndex].pData;
		}

		TKey getKeyByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return m_tKey;
			}

			if (m_pIterator == NULL)
			{
				resetIterator();
			}

			return *(m_pIterator[uiIndex].pKey);
		}

		void addItem(const TKey &key, TValue *pData)
		{
			if ( m_pHeadNode == NULL)
			{
				m_pHeadNode = new tagNode(key, pData);
				m_ulSize = 1;
				clearIterator();
			}
			else
			{
				addChild(m_pHeadNode, key, pData);
			}
		}

		void setItem(const TKey &key, TValue *pData)
		{
			tagNode* pNode = isItemExist(m_pHeadNode, key);

			if (pNode != NULL)
			{
				if (m_bFullControl)
				{
					if (pNode->pData != NULL)
					{
						delete pNode->pData;
					}
				}

				pNode->pData = pData;

				clearIterator();
			}
			else
			{
				addItem(key, pData);
			}
		}

		TValue* popItem(const TKey &key)
		{
			tagNode* pNode = isItemExist(m_pHeadNode, key);
			if (pNode == NULL)
			{
				return NULL;
			}

			TValue *pData = pNode->pData;
			pNode->pData = NULL;

			deleteItem(key);

			return pData;
		}

		BOOLEAN deleteItem(const TKey &key)
		{
			tagNode *pTemp;

			if ( m_pHeadNode == NULL )
			{
				return FALSE;
			}
			
			if ( compare(m_pHeadNode->key, key) == 0 )
			{
				pTemp = mergeTree(m_pHeadNode->pLeft, m_pHeadNode->pRight);

				deleteNode(m_pHeadNode);
				m_pHeadNode = NULL;

				if ( pTemp != NULL )
				{
					m_pHeadNode = pTemp;
					m_pHeadNode->pParent = NULL;
				}
				m_ulSize--;
				clearIterator();
				return TRUE;
			}			
			
			return deleteItemByDLR(m_pHeadNode, key);
		}

		void clear()
		{
			if (m_pIterator != NULL)
			{
				delete [] m_pIterator;
			}

			if (m_pHeadNode != NULL)
			{
				deleteTree(m_pHeadNode);
			}

			m_ulSize = 0;
			m_pHeadNode = NULL;			
			m_pIterator = NULL;
			m_ulItemCur = 0;
			m_ulIteratorCur = 0;
		}			

	protected:// method
		Map(const Map<TKey, TValue, TCompare>&){}
		Map<TKey, TValue, TCompare>& operator =(const Map<TKey, TValue, TCompare>&){return *this;}

	private:// method
		tagNode* isItemExist(tagNode *pNode, const TKey &key) const
		{
			if ( pNode == NULL )
			{
				return NULL;
			}

			if ( compare(pNode->key, key) == 0 )
			{
				return pNode;
			}

			if ( compare(pNode->key, key) > 0 )
			{
				return isItemExist(pNode->pLeft, key);
			}
			else
			{
				return isItemExist(pNode->pRight, key);
			}
		}

		void addChild(tagNode *pNode, const TKey &key, TValue *pData)
		{
			if (compare(pNode->key,key) ==0 )
			{
				return;
			}
			else if (compare(pNode->key,key) < 0)
			{
				if ( pNode->pRight == NULL )
				{
					pNode->pRight = new tagNode(key,pData);
					pNode->pRight->pParent = pNode;
					m_ulSize++;
					clearIterator();
					return;
				}
				else
				{
					return addChild(pNode->pRight,key, pData);
				}
			}
			else
			{
				if ( pNode->pLeft == NULL )
				{
					pNode->pLeft = new tagNode(key,pData);
					pNode->pLeft->pParent = pNode;
					m_ulSize++;
					clearIterator();
					return;
				}
				else
				{
					return addChild(pNode->pLeft,key, pData);
				}
			}
		}

		void createIteratorByLDR(tagNode *pNode)
		{
			if ( pNode == NULL )
			{
				return;
			}
			
			createIterator(pNode->pLeft);
			
			m_pIterator[m_ulIteratorCur].pKey = &pNode->key;
			m_pIterator[m_ulIteratorCur].pData = pNode->pData;
			m_ulIteratorCur++;
			
			createIterator(pNode->pRight);
		}

		BOOLEAN deleteItemByDLR(tagNode *pNode, const TKey &key)
		{
			tagNode *pTemp;

			if ( pNode == NULL )
			{
				return FALSE;
			}
			
			if ( compare(pNode->key, key) > 0 )
			{
				if ( pNode->pLeft != NULL )
				{
					if ( compare(pNode->pLeft->key, key) == 0 )
					{
						pTemp = mergeTree(pNode->pLeft->pLeft, pNode->pLeft->pRight);
						
						deleteNode(pNode->pLeft);
						pNode->pLeft = NULL;

						if( pTemp != NULL)
						{
							pNode->pLeft = pTemp;
							pNode->pLeft->pParent = pNode;
						}

						m_ulSize--;
						clearIterator();
						return TRUE;
					}
					
					if ( deleteItemByDLR(pNode->pLeft, key))
					{
						return TRUE;
					}
				}
			}
			else
			{
				if ( pNode->pRight != NULL )
				{
					if ( compare(pNode->pRight->key, key) == 0 )
					{
						pTemp = mergeTree(pNode->pRight->pLeft, pNode->pRight->pRight);

						deleteNode(pNode->pRight);
						pNode->pRight = NULL;

						if ( pTemp != NULL)
						{
							pNode->pRight = pTemp;
							pNode->pRight->pParent = pNode;
						}
						m_ulSize--;
						clearIterator();
						return TRUE;
					}
					
					if ( deleteItemByDLR(pNode->pRight, key))
					{
						return TRUE;
					}
				}
			}

			return FALSE;
		}

		tagNode* mergeTree(tagNode *pLeftNode,tagNode *pRightNode)
		{
			tagNode *pTemp = pRightNode;
			
			if ( pLeftNode == NULL && pRightNode == NULL)
			{
				return NULL;
			}
			
			if ( pLeftNode == NULL )
			{
				return pRightNode;
			}
			
			if ( pRightNode == NULL )
			{
				return pLeftNode;
			}
			
			while ( pTemp->pLeft != NULL )
			{
				pTemp = pTemp->pLeft;
			}
			
			pTemp->pLeft = pLeftNode;
			pLeftNode->pParent = pTemp;
			
			return pRightNode;
		}

		void clearIterator()
		{
			m_ulItemCur = 0;
			m_ulIteratorCur = 0;

			if (m_pIterator != NULL)
			{
				delete [] m_pIterator;
				m_pIterator = NULL;
			}
		}

		void createIterator(tagNode *pNode)
		{
			if ( pNode == NULL )
			{
				return;
			}

			createIterator(pNode->pLeft);

			m_pIterator[m_ulIteratorCur].pKey = &pNode->key;
			m_pIterator[m_ulIteratorCur].pData = pNode->pData;
			m_ulIteratorCur++;

			createIterator(pNode->pRight);
		}

		void deleteTree(tagNode *pNode)
		{
			if ( pNode == NULL )
			{
				return;
			}

			if (pNode->pLeft != NULL)
			{
				deleteTree(pNode->pLeft);
			}

			if (pNode->pRight != NULL)
			{
				deleteTree(pNode->pRight);
			}

			if ( pNode->pParent != NULL )
			{
				if (pNode->pParent->pLeft == pNode)
				{
					pNode->pParent->pLeft = NULL;
				}
				else
				{
					pNode->pParent->pRight = NULL;
				}
			}

			deleteNode(pNode);
		}

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
		tagNode		*m_pHeadNode;
		tagPair 	*m_pIterator;
		ULONG		m_ulSize;		
		ULONG		m_ulItemCur;
		ULONG		m_ulIteratorCur;
		TCompare	compare;
		TKey		m_tKey;
	};
}