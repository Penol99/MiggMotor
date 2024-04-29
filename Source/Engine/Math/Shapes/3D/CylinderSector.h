#pragma once
#include "Matrix4x4.h"
#include "Cylinder.h"

namespace mi
{
	class CylinderSector
	{
	public:

		CylinderSector() = default;
		CylinderSector(const Vector3f& aCenter, const float aRadius, const float aHeight, const float anAngle);

		Vector3f GetCenter() const;
		void SetCenter(const Vector3f& aCenter);

		Cylinder myCylinder;
		float myAngle;
	};
}