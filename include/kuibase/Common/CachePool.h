/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	CachePool	
	*			It is a cache pool for CacheObject, maybe you don't use it in your project.
	*/
	template<typename TKey, typename TValue, typename TCompare>
	class CachePool : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CachePool()
			: m_uiPoolSize(10)
		{
			;
		}

		virtual ~CachePool()
		{
			;
		}

		void setPoolSize(UINT uiPoolSize)
		{
			m_uiPoolSize = uiPoolSize; //< 10 ? 10 : uiPoolSize;
		}

		void addCacheObject(const TKey &key, CacheObject *pobj)
		{
			if (pobj == NULL || m_uiPoolSize == 0 )
			{
				return;
			}

			pobj->increaseFrequency();

			removeOldCacheObject();

			m_mapCacheObject.addItem(key, ObjectPtr<TValue>((TValue*)pobj));
		}

		ObjectPtr<TValue> getCacheObject(const TKey &key)
		{
			ObjectPtr<TValue> &ptr = m_mapCacheObject[key];

			if (ptr.isNotNull())
			{
				ptr->increaseFrequency();
			}

			return ptr;
		}

		void deleteCacheObject(const TKey &key)
		{
			m_mapCacheObject.deleteItem(key);
		}

		UINT getCount()
		{
			return m_mapCacheObject.size();
		}

		void clear()
		{
			m_mapCacheObject.clear();
		}

	protected:// method		
		CachePool(const CachePool<TKey, TValue, TCompare>&){}
		CachePool<TKey, TValue, TCompare>& operator =(const CachePool<TKey, TValue, TCompare>&){return *this;}		

	private:// method	
		void removeOldCacheObject()
		{
			if (m_uiPoolSize == 0 || m_mapCacheObject.size() < m_uiPoolSize)
			{
				return;
			}

			UINT uiMinFre = 0xFFFFFFFF;
			UINT uiMinTime = 0xFFFFFFFF;
			TKey *pKey = NULL;

			m_mapCacheObject.resetIterator();				
			while (m_mapCacheObject.hasMore()) 
			{
				MapPair<TKey, ObjectPtr<TValue> > mapPair = m_mapCacheObject.nextKeyAndValue();

				CacheObject* p = mapPair.pData->getRaw();

				if (p->getFrequency() < uiMinFre)
				{
					uiMinFre = p->getFrequency();
					uiMinTime = p->getTime();
					pKey = mapPair.pKey;
				}
				else if (p->getFrequency() == uiMinFre)
				{
					if (p->getTime() < uiMinTime)
					{
						uiMinFre = p->getFrequency();
						uiMinTime = p->getTime();
						pKey = mapPair.pKey;
					}
				}
			}

			if (pKey != NULL)
			{
				m_mapCacheObject.deleteItem(*pKey);
			}
		}

	protected:// property	

	private:// property
		UINT	m_uiPoolSize;

		Map2<TKey, ObjectPtr<TValue>, TCompare>	m_mapCacheObject;
	};
}