#include "engine_pch.h"
#include "CylinderSector.h"

namespace mi
{
	CylinderSector::CylinderSector(const Vector3f& aCenter, const float aHeight, const float aRadius, const float anAngle)
		: myCylinder(Cylinder(aCenter, aHeight, aRadius))
		, myAngle(anAngle)
	{
		myCylinder.myMatrix.SetPosition(aCenter);
	}

	Vector3f CylinderSector::GetCenter() const
	{
		return myCylinder.GetCenter();
	}

	void CylinderSector::SetCenter(const Vector3f& aCenter)
	{
		myCylinder.SetCenter(aCenter);
	}
}

