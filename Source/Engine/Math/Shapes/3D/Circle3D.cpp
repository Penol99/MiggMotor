#include "engine_pch.h"
#include "Circle3D.h"

namespace mi
{
	Circle3D::Circle3D(const Vector3f& aCenter, const float aRadius)
		: myRadius(aRadius)
	{
		SetCenter(aCenter);
	}

	const Vector3f& Circle3D::GetCenter() const
	{
		return myMatrix.GetPosition();
	}

	float Circle3D::GetRadius() const
	{
		return myRadius;
	}

	const Matrix4x4f& Circle3D::GetMatrix() const
	{
		return myMatrix;
	}

	void Circle3D::SetCenter(const Vector3f& aCenter)
	{
		myMatrix.SetPosition(aCenter);
	}

	void Circle3D::SetRadius(const float aRadius)
	{
		myRadius = aRadius;
	}

}
