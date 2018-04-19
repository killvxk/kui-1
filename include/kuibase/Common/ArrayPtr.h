/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	ArrayPtr	
	*			It is a smart pointer for general c++ array type (not Array/Array2 type). 
	*			If you set a c++ array type pointer into this smart array pointer, it will take charge of the lifesycle of that c++ array.  
	*/
	template<typename T>
	class ArrayPtr
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		ArrayPtr() 
			: m_p(NULL)
			, m_uiSize(0)
		{
			;
		}
		
		ArrayPtr(T *pSrc, UINT uiSize)
			: m_p(pSrc)
			, m_uiSize(uiSize)
		{		
			;
		}
		
		virtual ~ArrayPtr()
		{
			free();
		}

		UINT size()
		{
			return m_uiSize;
		}

		void attach(T *pSrc, UINT uiSize)
		{
			if (m_p == pSrc)
			{
				return;
			}

			free();

			m_p = pSrc;
			m_uiSize = uiSize;
		}

		BOOLEAN resize(UINT uiSize)
		{
			if (uiSize <= m_uiSize)
			{
				return TRUE;
			}

			T *p = new T[uiSize];
			if (p == NULL)
			{
				return FALSE;
			}

			T *pHead = m_p;
			T *pTail = m_p + m_uiSize;
			T *pCur = p;
			while (pHead < pTail)
			{
				*pCur++ = *pHead++;
			}

			free();

			m_p = p;
			m_uiSize = uiSize;

			return TRUE;
		}	

		BOOLEAN isNull()
		{
			return m_p == NULL ? TRUE : FALSE;
		}
		
		BOOLEAN operator !=(T *pSrc) const
		{
			return (m_p != pSrc) ? TRUE : FALSE;
		}

		BOOLEAN operator !=(ArrayPtr<T> &pSrc) const
		{
			return (m_p != pSrc.m_p) ? TRUE : FALSE;
		}

		BOOLEAN operator ==(T *pSrc) const
		{
			return (m_p == pSrc) ? TRUE : FALSE;
		}
		
		BOOLEAN operator ==(ArrayPtr<T> &pSrc) const
		{
			return (m_p == pSrc.m_p) ? TRUE : FALSE;
		}		
		
		operator T*() const
		{
			return (m_p);
		}

		T& operator [](UINT uiIndex)
		{
			if (uiIndex >= m_uiSize - 1)
			{
				return m_p[m_uiSize - 1];
			}
			else
			{
				return m_p[uiIndex];
			}
		}
		
	protected:// method
		ArrayPtr(const ArrayPtr<T>&){}
		ArrayPtr<T>& operator =(const ArrayPtr<T>&){return *this;}
		ArrayPtr<T>& operator =(T *pSrc){return *this;}

	private:// method
		void free()
		{
			if (m_p != NULL)
			{
				delete [] m_p;
				m_p = NULL;
				m_uiSize = 0;
			}
		}
		
	protected:// property

	private:// property
		T		*m_p;
		UINT	m_uiSize;
	};
}