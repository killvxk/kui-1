#pragma once

namespace OpenHMI
{
	template <typename T>
	class CPool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CPool();
		CPool(UINT uiCapacity);
		~CPool();

		T*		get();
		void	release(T* t);
		void	release();
	
	protected:// method	
		CPool(const CPool &){}
		CPool &operator = (const CPool &){return *this;}

	private:// method

	protected:// property

	private:// property
		UINT		m_uiCapacity;
		List<T>		m_listFree;
	};

	template <typename T>
	CPool<T>::CPool()
		: m_uiCapacity(100)
	{
		;
	}

	template <typename T>
	CPool<T>::CPool(UINT uiCapacity)
		: m_uiCapacity(uiCapacity)
	{
		;
	}

	template <typename T>
	CPool<T>::~CPool()
	{
		release();
	}

	template <typename T>
	T* CPool<T>::get()
	{
		T* pReturned = NULL;
		if ( m_listFree.isEmpty() )
		{
			pReturned = new T();
		}
		else
		{
			pReturned = m_listFree.getFirst();
			m_listFree.popFirst();
		}
		
		return pReturned;
	}

	template <typename T>
	void CPool<T>::release(T* t)
	{
		if ( m_listFree.size() >= m_uiCapacity )
		{
			delete t;
			return;
		}

		m_listFree.addLast(t);
	}

	template <typename T>
	void CPool<T>::release()
	{
		m_listFree.clear();
	}
}

