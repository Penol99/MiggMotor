#include "engine_pch.h"
#include "Sphere.h"

namespace mi
{
	Sphere::Sphere(const Vector3f& aPoint, const float aRadius)
		: myCenter(aPoint)
		, myRadius(aRadius)
	{
	}
	Sphere::Sphere(const float aRadius)
		: myRadius(aRadius)
	{
	}
}

