/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DRay : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		C3DRay();
		C3DRay(const C3DRay &src);
		C3DRay(const C3DVector &vtOrigin, const C3DVector &vtDirection);
		virtual ~C3DRay();

		void		setOrigin(const C3DVector &vtValue)		{ m_vtOrigin = vtValue; }
		C3DVector	getOrigin() const						{ return m_vtOrigin; }
		void		setDirection(const C3DVector &vtValue)	{ m_vtDirection = vtValue; m_vtDirection.normalise(); }
		C3DVector	getDirection() const					{ return m_vtDirection; }

		C3DVector	getPoint(FLOAT fDistance) const;

		Pair<BOOLEAN, FLOAT> intersects(const C3DPlane &plane) const;
		Pair<BOOLEAN, FLOAT> intersects(const C3DBox &box) const;

		C3DRay&		operator =(const C3DRay &src);
		BOOLEAN		operator ==(const C3DRay &src) const;
		BOOLEAN		operator !=(const C3DRay &src) const;

		C3DVector	operator *(FLOAT fDistance) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		C3DVector	m_vtOrigin;
		C3DVector	m_vtDirection;
	};
}