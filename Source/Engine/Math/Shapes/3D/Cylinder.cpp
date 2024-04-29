#include "engine_pch.h"
#include "Cylinder.h"

namespace mi
{
	Cylinder::Cylinder(const Vector3f& aCenter, const float aHeight, const float aRadius)
		: myHeight(aHeight)
		, myRadius(aRadius)
	{
		myMatrix.SetPosition(aCenter);
	}

	Vector3f Cylinder::GetCenter() const
	{
		return myMatrix.GetPosition();
	}

	void Cylinder::SetCenter(const Vector3f& aCenter)
	{
		myMatrix.SetPosition(aCenter);
	}

}
