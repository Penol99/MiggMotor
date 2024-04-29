#pragma once
#include "Matrix4x4.h"

namespace mi
{
	
	class Cylinder
	{
	public:
		Cylinder() = default;
		Cylinder(const Vector3f& aCenter, const float aHeight, const float aRadius);

		Vector3f GetCenter() const;
		void SetCenter(const Vector3f& aCenter);

		Matrix4x4f myMatrix;
		float myHeight, myRadius;
	};
}