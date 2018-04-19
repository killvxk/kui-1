/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	CachePool2	
	*			It is a cache pool for CacheObject, maybe you don't use it in your project.
	*/
	class OPENHMI_API CachePool2 : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CachePool2() 
			: m_uiPoolSize(10)
		{
			;
		}

		~CachePool2()
		{
			;
		}

		void setPoolSize(UINT uiPoolSize)
		{
			m_uiPoolSize = uiPoolSize < 10 ? 10 : uiPoolSize;
		}

		INT addCacheObject(const String& strKey, CacheObject *pobj)
		{
			if (pobj == NULL)
			{
				return -1;
			}

			pobj->increaseFrequency();

			removeOldCacheObject();

			m_mapCacheObject.addItem(strKey, pobj);
			m_aryCacheObject.addItem(pobj);

			pobj->setName(strKey);
			pobj->setID(m_aryCacheObject.size() - 1);

			return pobj->getID();
		}

		CacheObject* getCacheObject(const String& strKey)
		{
			CacheObject *pobj = m_mapCacheObject[strKey];

			if (pobj != NULL)
			{
				pobj->increaseFrequency();
			}

			return pobj;
		}

		CacheObject* getCacheObject(UINT uiIndex)
		{
			if ( uiIndex < m_aryCacheObject.size() )
			{
				CacheObject *pobj = m_aryCacheObject[uiIndex];
				if (pobj != NULL)
				{
					pobj->increaseFrequency();
				}

				return pobj;
			}

			return NULL;
		}		

		void deleteCacheObject(const String& strKey)
		{
			CacheObject *pobj = m_mapCacheObject[strKey];
			if (pobj == NULL)
			{
				return;
			}

			INT iIndex = pobj->getID();

			m_mapCacheObject.deleteItem(strKey);
			m_aryCacheObject[iIndex] = NULL;
		}

		void deleteCacheObject(UINT uiIndex)
		{
			if ( uiIndex < m_aryCacheObject.size() )
			{
				CacheObject* pObj = m_aryCacheObject[uiIndex];
				if ( pObj )
				{
					m_mapCacheObject.deleteItem(pObj->getName() );
					m_aryCacheObject[uiIndex] = NULL;
				}
			}
		}

		UINT getCount()
		{
			return m_mapCacheObject.size();
		}
		
		void clear()
		{
			m_mapCacheObject.clear();
			m_aryCacheObject.clear();
		}		
	
	protected:// method	
		CachePool2(const CachePool2 &){}
		CachePool2 &operator = (const CachePool2 &){return *this;}

	private:// method
		void removeOldCacheObject()
		{
			if (m_mapCacheObject.size() < m_uiPoolSize)
			{
				return;
			}

			UINT uiMinFre = 0xFFFFFFFF;
			UINT uiMinTime = 0xFFFFFFFF;
			String *pKey = NULL;
			CacheObject* pValue = NULL;

			m_mapCacheObject.resetIterator();				
			while (m_mapCacheObject.hasMore()) 
			{
				MapPair<String, CacheObject> mapPair = m_mapCacheObject.nextKeyAndValue();

				if (mapPair.pData->getFrequency() < uiMinFre)
				{
					uiMinFre = mapPair.pData->getFrequency();
					uiMinTime = mapPair.pData->getTime();
					pKey = mapPair.pKey;
					pValue = mapPair.pData;
				}
				else if (mapPair.pData->getFrequency() == uiMinFre)
				{
					if (mapPair.pData->getTime() < uiMinTime)
					{
						uiMinFre = mapPair.pData->getFrequency();
						uiMinTime = mapPair.pData->getTime();
						pKey = mapPair.pKey;
						pValue = mapPair.pData;
					}
				}
			}

			if (pKey != NULL && pValue != NULL)
			{
				m_aryCacheObject[pValue->getID()] = NULL;
				m_mapCacheObject.deleteItem(*pKey);
			}
		}

	protected:// property

	private:// property
		UINT	m_uiPoolSize;

		Map<String, CacheObject, tagStringCompare>	m_mapCacheObject;
		Array2<CacheObject*>						m_aryCacheObject;		
	};
}