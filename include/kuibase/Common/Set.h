/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	Set	
	*			It is a common set class.
	*			The type TKey must support "=" operation.
	*			Generally you can use tagNumCompare or tagStringCompare as TKey comparing.
	*/
	template<typename TKey,typename TCompare>
	class Set : protected Map<TKey, INT, TCompare>
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		typedef Map<TKey, INT, TCompare>	_Base;

	public:// method
		Set()
		{

		}

		virtual ~Set()
		{
			clear();
		}

		/**
		* @brief	Begin to access to each item of the set.
		*			Used with hasMore and next functions together.
		*			Example:
		*
		*			Set<TKey, TCompare> temp;
		*			temp.resetIterator();
		*			while (temp.hasMore())
		*			{ TKey tKey = temp.next(); }
		*			
		* @return	void
		*/
		void resetIterator()
		{
			_Base::resetIterator();
		}

		BOOLEAN hasMore()
		{
			return _Base::hasMore();
		}

		TKey next()
		{
			return _Base::nextKey();
		}

		BOOLEAN isExist(const TKey &key) const
		{
			return _Base::isExist(key);
		}

		BOOLEAN isEmpty() const
		{
			return _Base::isEmpty();
		}

		ULONG size() const
		{
			return _Base::size();
		}

		void addItem(const TKey &key)
		{
			_Base::addItem(key, NULL);
		}
		
		BOOLEAN deleteItem(const TKey &key)
		{
			return _Base::deleteItem(key);
		}

		void clear()
		{
			_Base::clear();
		}

	protected:// method
		Set(const Set<TKey, TCompare>&){}
		Set<TKey, TCompare>& operator =(const Set<TKey, TCompare>&){return *this;}

	private:// method

	protected:// property

	private:// property

	};
}