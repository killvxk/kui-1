/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	Map2	
	*			It is a common map class and its value part member must be an instance of class TValue.
	*			The type TKey must support "=" operation.
	*			The type TValue must support "=" operation.
	*			Generally you can use tagNumCompare or tagStringCompare as TKey comparing.
	*/
	template<typename TKey,typename TValue, typename TCompare>
	class Map2 : public Object
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
			TValue	data;
			tagNode *pLeft;
			tagNode *pRight;
			tagNode *pParent;

			tagNode(const TKey &tKey, const TValue &tData)
				: key(tKey)
				, data(tData)
				, pLeft(NULL)
				, pRight(NULL)
				, pParent(NULL)
			{
				;
			}
		};

	public:// method
		Map2()
			: m_pHeadNode(NULL)
			, m_pIterator(NULL)
			, m_ulSize(0)
			, m_ulItemCur(0)
			, m_ulIteratorCur(0)
		{
			;
		}

		virtual ~Map2()
		{
			clear();
		}

		/**
		* @brief	Begin to access to each item of the map2.
		*			Used with hasMore and next (nextKey, nextKeyAndValue) functions together.
		*			Example:
		*
		*			Map2<TKey, TValue, TCompare> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ TValue &tValue = temp.next(); }
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

		TValue& next()
		{
			return *m_pIterator[m_ulItemCur++].pData;
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

		TValue& operator[](const TKey &key)
		{
			return getValueByKey(key);
		}

		TValue operator[](const TKey &key) const
		{
			return getValueByKey(key);
		}

		TValue getValueByKey(const TKey &key) const
		{
			tagNode* pNode = isItemExist(m_pHeadNode, key);

			return pNode == NULL ? m_tValue : pNode->data;
		}

		TValue& getValueByKey(const TKey &key)
		{
			tagNode* pNode = isItemExist(m_pHeadNode, key);

			return pNode == NULL ? m_tValue : pNode->data;
		}

		TValue getValueByIndex(UINT uiIndex)
		{
			if (uiIndex >= m_ulSize)
			{
				return m_tValue;
			}

			if (m_pIterator == NULL)
			{
				resetIterator();
			}

			return *(m_pIterator[uiIndex].pData);
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

		void addItem(const TKey &key, const TValue &data)
		{
			if ( m_pHeadNode == NULL)
			{
				m_pHeadNode = new tagNode(key, data);
				m_ulSize = 1;
				clearIterator();
			}
			else
			{
				addChild(m_pHeadNode, key, data);
			}
		}

		void setItem(const TKey &key, const TValue &data)
		{
			tagNode* pNode = isItemExist(m_pHeadNode, key);

			if (pNode != NULL)
			{
				pNode->data = data;
				clearIterator();
			}
			else
			{
				addItem(key, data);
			}
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

				if( pTemp != NULL )
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
		Map2(const Map2<TKey, TValue, TCompare>&){}
		Map2<TKey, TValue, TCompare>& operator =(const Map2<TKey, TValue, TCompare>&){return *this;}
		
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

		void addChild(tagNode *pNode, const TKey &key, const TValue &data)
		{
			if (compare(pNode->key,key) ==0 )
			{
				return;
			}
			else if (compare(pNode->key, key) < 0)
			{
				if ( pNode->pRight == NULL )
				{
					pNode->pRight = new tagNode(key, data);
					pNode->pRight->pParent = pNode;
					m_ulSize++;
					clearIterator();
					return;
				}
				else
				{
					return addChild(pNode->pRight, key, data);
				}
			}
			else
			{
				if ( pNode->pLeft == NULL )
				{
					pNode->pLeft = new tagNode(key, data);
					pNode->pLeft->pParent = pNode;
					m_ulSize++;
					clearIterator();
					return;
				}
				else
				{
					return addChild(pNode->pLeft, key, data);
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
			m_pIterator[m_ulIteratorCur].pData = &pNode->data;
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
			m_pIterator[m_ulIteratorCur].pData = &pNode->data;
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
			delete pNode;
		}

	protected:// property

	private:// property
		tagNode			*m_pHeadNode;
		tagPair 		*m_pIterator;
		ULONG			m_ulSize;		
		ULONG			m_ulItemCur;
		ULONG			m_ulIteratorCur;
		TCompare		compare;		
		TKey			m_tKey;
		TValue			m_tValue;
	};
}