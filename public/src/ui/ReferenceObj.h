#pragma once


class ReferenceObj
{
public:

	ReferenceObj()
	{
		m_ref = 0;
	}

	virtual ~ReferenceObj()
	{

	}

	virtual void AddRef()
	{
		++m_ref;
	}

	virtual int Release()
	{
		if (m_ref == 1)
		{
			Destroy();
			delete this;
			return 0;
		}
		m_ref--;
		return m_ref;
	}

	virtual void Destroy()
	{

	}

	int					m_ref;
};

// 自动引用计数对象
template <typename T>
class AutoRef
{
public:
	AutoRef() { m_p = NULL; }

	AutoRef(T* p) { m_p = p; if (m_p) m_p->AddRef(); }

	AutoRef(int p) { m_p = NULL; }

	AutoRef(AutoRef<T>& obj)
	{
		m_p = obj.m_p;
		if (m_p)
		{
			m_p->AddRef();
		}
	}

	~AutoRef()
	{
		if (m_p)
		{
			T* p2 = m_p;
			m_p = NULL;
			p2->Release();
		}
	}

	T& operator *() { return *m_p; }

	T* operator ->() { return m_p; }

	operator T*() const { return m_p; }

	operator bool() const { return m_p != NULL; }

	T* ptr()
	{
		return m_p;
	}

	template <typename T2>
	operator AutoRef<T2>() const
	{
		return AutoRef<T2>(m_p);
	}

	AutoRef<T>& operator = (AutoRef<T>& obj)
	{
		if (m_p)
		{
			T* p2 = m_p;
			m_p = NULL;
			p2->Release();
		}

		m_p = obj.m_p;
		if (m_p) m_p->AddRef();
		return *this;
	}

	AutoRef<T>& operator = (T* p)
	{
		if (m_p)
		{
			T* p2 = m_p;
			m_p = NULL;
			p2->Release();
		}

		m_p = p;
		if (m_p) m_p->AddRef();
		return *this;
	}

	AutoRef<T>& operator = (int p)
	{
		if (m_p)
		{
			T* p2 = m_p;
			m_p = NULL;
			p2->Release();
		}
		return *this;
	}

	bool operator == (AutoRef<T>& obj) const
	{
		return m_p == obj.m_p;
	}

	bool operator == (T* p) const
	{
		return m_p == p;
	}

	bool operator == (int p) const
	{
		return m_p == 0;
	}

	bool operator != (AutoRef<T>& obj) const
	{
		return m_p != obj.m_p;
	}

	bool operator != (T* p) const
	{
		return m_p != p;
	}

	bool operator != (int p) const
	{
		return m_p != NULL;
	}

	void Release()
	{
		if (m_p)
		{
			T* p2 = m_p;
			m_p = NULL;
			p2->Release();
		}
	}

	void Delete()
	{
		if (m_p)
		{
			T* p2 = m_p;
			m_p = NULL;
			delete p2;
		}
	}

	T* Detach()
	{
		if (m_p)
		{
			T* p = m_p;
			m_p = NULL;
			return p;
		}
		return NULL;
	}

private:

	T* m_p;
};


class RefGuard
{
public:

	RefGuard(ReferenceObj* p)
	{
		m_p = p;
		if (m_p)
		{
			m_p->AddRef();
		}
	}

	~RefGuard()
	{
		if (m_p)
		{
			m_p->Release();
			m_p = NULL;
		}
	}

	int RefCount()
	{
		if (m_p)
		{
			return m_p->m_ref;
		}
		return 0;
	}
	ReferenceObj* m_p;
};


