/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API C3DPlane : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EPlaneSide
		{
			PS_NO_SIDE			= 0,
			PS_POSITIVE_SIDE,
			PS_NEGATIVE_SIDE,
			PS_BOTH_SIDE
		};

	private:// embed class or struct or enum

	public:// method
		C3DPlane();
		C3DPlane(const C3DPlane &src);
		C3DPlane(const C3DVector &vtNormalize, FLOAT fDistance);
		C3DPlane(const C3DVector &vtNormalize, const C3DVector &vtPoint1);
		C3DPlane(const C3DVector &vtPoint1, const C3DVector &vtPoint2, const C3DVector &vtPoint3);
		virtual ~C3DPlane();

		void		setNormalize(const C3DVector &vtValue)	{ m_vtNormalize = vtValue; m_vtNormalize.normalise(); }
		C3DVector	getNormalize() const					{ return m_vtNormalize; }
		void		setDistance(FLOAT fValue)				{ m_fDistance = fValue; }
		FLOAT		getDistance() const						{ return m_fDistance; }

		EPlaneSide	getSide(const C3DVector &vtPoint) const;
		EPlaneSide	getSide(const C3DBox &box) const;
		EPlaneSide	getSide(const C3DVector &vtCentre, const C3DVector &vtHalfSize) const;

		void		redefine(const C3DVector &vtNormalize, const C3DVector &vtPoint1);
		void		redefine(const C3DVector &vtPoint1, const C3DVector &vtPoint2, const C3DVector &vtPoint3);

		FLOAT		getDistance (const C3DVector &vtPoint) const;
		C3DVector	projectVector(const C3DVector &vt) const;

		C3DPlane&	operator =(const C3DPlane &src);
		BOOLEAN		operator ==(const C3DPlane &src) const;
		BOOLEAN		operator !=(const C3DPlane &src) const;

	protected:// method

	private:// method

	protected:// property

	private:// property
		C3DVector	m_vtNormalize;
		FLOAT		m_fDistance;
	};
}