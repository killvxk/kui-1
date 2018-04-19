/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	template<typename TKey, typename TValue>
	class MapPair
	{
	public:
		TKey	*pKey;
		TValue	*pData;

		MapPair(TKey *pTKey, TValue *pTData) 
			: pKey(pTKey)
			, pData(pTData)
		{
			;
		}	

		MapPair() : pKey(NULL), pData(NULL)
		{

		}
	};

	template<typename T1, typename T2>
	class Pair : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		Pair(T1 t1, T2 t2)
			: m_t1(t1)
			, m_t2(t2)
		{
			;
		}

		Pair(const Pair<T1, T2> &src)
		{
			m_t1 = src.m_t1;
			m_t2 = src.m_t2;
		}

		virtual ~Pair()
		{
			;
		}

		T1& t1()
		{
			return m_t1;
		}

		T2& t2()
		{
			return m_t2;
		}

		Pair<T1, T2>& operator =(const Pair<T1, T2> &src)
		{
			m_t1 = src.m_t1;
			m_t2 = src.m_t2;

			return *this;
		}

	protected:// method		

	private:// method

	protected:// property

	private:// property
		T1	m_t1;
		T2	m_t2;
	};
}