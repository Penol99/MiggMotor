#pragma once
#include "Vector.h"

namespace mi
{
	class Sphere
	{
	public:
		
		Sphere() = default;
		Sphere(const Vector3f& aPoint, const float aRadius);
		Sphere(const float aRadius);

		Vector3f myCenter;
		float myRadius;
	};
}