/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	ObjectPtr	
	*			It is a smart object pointer for those classes inherited from Object class.
	*			If you set a Object type pointer into this smart object pointer, it will take charge of the lifecycle of that Object.  
	*/
	template<typename T>
	class ObjectPtr
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		ObjectPtr() 
			: m_p(NULL)
		{
			;
		}

		explicit ObjectPtr(T *pSrc)
		{
			m_p =  pSrc;
			if (m_p != NULL)
			{
				m_p->addRef();
			}
		}

		ObjectPtr(ObjectPtr<T> &pSrc)
		{
			m_p = pSrc.m_p;
			if (m_p != NULL)
			{
				m_p->addRef();
			}
		}

		virtual ~ObjectPtr()
		{
			free();
		}

		void attach(T *pSrc)
		{
			if (m_p == pSrc)
			{
				return;
			}

			free();
			
			m_p = pSrc;
			if (m_p != NULL)
			{
				m_p->addRef();
			}
		}

		BOOLEAN isNull()
		{
			return m_p == NULL ? TRUE : FALSE;
		}

		BOOLEAN isNotNull()
		{
			return m_p != NULL ? TRUE : FALSE;
		}

		void setNull()
		{
			free();
		}

		T* getRaw()
		{
			return m_p;
		}

		ObjectPtr<T>& operator =(T *pSrc)
		{
			attach(pSrc);
			return *this;
		}

		ObjectPtr<T>& operator =(ObjectPtr<T> &pSrc)
		{
			attach(pSrc.m_p);
			return *this;
		}

		BOOLEAN operator !=(T* pSrc) const
		{
			return (m_p != pSrc) ? TRUE : FALSE;
		}

		BOOLEAN operator !=(ObjectPtr<T> &pSrc) const
		{
			return (m_p != pSrc.m_p) ? TRUE : FALSE;
		}

		BOOLEAN operator ==(T* pSrc) const
		{
			return (m_p == pSrc) ? TRUE : FALSE;
		}

		BOOLEAN operator ==(ObjectPtr &pSrc) const
		{
			return (m_p == pSrc.m_p) ? TRUE : FALSE;
		}

		operator T*() const
		{
			return (m_p);
		}

		T* operator ->() const
		{
			return (m_p);
		}

		T& operator *() const
		{
			return *m_p;
		}

	protected:// method

	private:// method
		void free()
		{
			if (m_p != NULL)
			{
				m_p->release();
				m_p = NULL;
			}
		}

	protected:// property

	private:// property
		T	*m_p;
	};
}