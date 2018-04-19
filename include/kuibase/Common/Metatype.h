/*@Author Zeki.Yan*/
#pragma once

namespace OpenHMI
{
	/**
	* @class	Metatype	
	*			This class is a wrapper class for all the c++ meta type.
	*			It is used to consummate the mechanism of comunication between framework and service.
	*			If a service API need c++ meta type as parameter, please use this class instead. 
	*			Then the OpenHMI SDK will free it automatically for it inherited from Object.
	*/
	template <typename T>
	class Metatype : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		Metatype()
		{
			;
		}

		explicit Metatype(T tValue)
		{
			m_tValue = tValue;
		}

		explicit Metatype(Metatype<T> &t) 
		{
			m_tValue = t.m_tValue;
		}

		~Metatype()
		{
			;
		}	

		Metatype<T>& operator =(T tValue)
		{
			m_tValue = tValue;

			return *this;
		}

		Metatype<T>& operator =(Metatype<T> &t)
		{
			m_tValue = t.m_tValue;

			return *this;
		}

		BOOLEAN operator != (T tValue) const
		{
			return (m_tValue != tValue) ? TRUE : FALSE;
		}

		BOOLEAN operator != (Metatype<T> &t) const
		{
			return (m_tValue != t.m_tValue) ? TRUE : FALSE;
		}

		BOOLEAN operator == (T tValue) const
		{
			return (m_tValue == tValue) ? TRUE : FALSE;
		}		

		BOOLEAN operator == (Metatype<T> &t) const
		{
			return (m_tValue == t.m_tValue) ? TRUE : FALSE;
		}

		BOOLEAN operator < (T tValue) const
		{
			return (m_tValue < tValue) ? TRUE : FALSE;
		}		

		BOOLEAN operator < (Metatype<T> &t) const
		{
			return (m_tValue < t.m_tValue) ? TRUE : FALSE;
		}

		BOOLEAN operator <= (T tValue) const
		{
			return (m_tValue <= tValue) ? TRUE : FALSE;
		}		

		BOOLEAN operator <= (Metatype<T> &t) const
		{
			return (m_tValue <= t.m_tValue) ? TRUE : FALSE;
		}

		BOOLEAN operator > (T tValue) const
		{
			return (m_tValue > tValue) ? TRUE : FALSE;
		}		

		BOOLEAN operator > (Metatype<T> &t) const
		{
			return (m_tValue > t.m_tValue) ? TRUE : FALSE;
		}

		BOOLEAN operator >= (T tValue) const
		{
			return (m_tValue >= tValue) ? TRUE : FALSE;
		}		

		BOOLEAN operator >= (Metatype<T> &t) const
		{
			return (m_tValue >= t.m_tValue) ? TRUE : FALSE;
		}

		operator T() const
		{
			return (m_tValue);
		}

		operator T*() const
		{
			return (&m_tValue);
		}

	protected:// method

	private:// method

	protected:// property

	private:// property
		T	m_tValue;
	};

	typedef Metatype<BYTE>		CByte;
	typedef Metatype<CHAR>		CChar;
	typedef Metatype<SHORT>		CShort;
	typedef Metatype<INT>		CInt;
	typedef Metatype<LONG>		CLong;
	typedef Metatype<USHORT>	CUShort;
	typedef Metatype<UINT>		CUInt;
	typedef Metatype<ULONG>		CULong;
	typedef Metatype<FLOAT>		CFloat;
	typedef Metatype<DOUBLE>	CDouble;
	typedef Metatype<BOOLEAN>	CBoolean;
}