/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DBox : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DBox();
		C3DBox(const C3DBox &src);
		C3DBox(const C3DVector &vtPoint1, const C3DVector &vtPoint2);
		virtual ~C3DBox();

		void		setPoint1(const C3DVector &vtValue)	{ m_vtPoint1 = vtValue; }
		C3DVector	getPoint1() const					{ return m_vtPoint1; }
		void		setPoint2(const C3DVector &vtValue)	{ m_vtPoint2 = vtValue; }
		C3DVector	getPoint2() const					{ return m_vtPoint2; }

		C3DBox&		operator =(const C3DBox &src);
		BOOLEAN		operator ==(const C3DBox &src) const;
		BOOLEAN		operator !=(const C3DBox &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		C3DVector	m_vtPoint1;
		C3DVector	m_vtPoint2;
	};
}