#include "engine_pch.h"
#include "Plane.h"

namespace mi
{
	Plane::Plane(const Vector3f& aPoint, const Vector3f& aNormal)
		: myPoint(aPoint)
		, myNormal(aNormal)
	{
	}

	void Plane::InitWith3Points(const Vector3f& aPoint1, const Vector3f& aPoint2, const Vector3f& aPoint3)
	{
		myPoint = aPoint1;
		myNormal = (aPoint2 - aPoint1).Cross(aPoint3 - aPoint1).GetNormalized();
	}

	void Plane::SetPoint(const Vector3f& aPoint)
	{
		myPoint = aPoint;
	}

	void Plane::SetNormal(const Vector3f& aNormal)
	{
		myNormal = aNormal.GetNormalized();
	}

	const Vector3f& Plane::GetPoint() const
	{
		return myPoint;
	}

	const Vector3f& Plane::GetNormal() const
	{
		return myNormal;
	}

	bool Plane::IsPointOnNormalSide(const Vector3f& aPoint) const
	{
		const float dot = (aPoint - myPoint).Dot(myNormal);
		return dot > 0;
	}

	bool Plane::IsPointInside(const Vector3f& aPoint) const
	{
		return !IsPointOnNormalSide(aPoint);
	}

	float Plane::GetXPos(const float aYPos, const float aZPos) const
	{
		if (myNormal.x == 0.f)
		{
			return std::numeric_limits<float>::infinity();
		}
		float x = myPoint.x + (-(myNormal.y * (aYPos - myPoint.y) + myNormal.z * (aZPos - myPoint.z)) / myNormal.x);
		return x;
	}

	float Plane::GetYPos(const float aXPos, const float aZPos) const
	{
		if (myNormal.y == 0.f)
		{
			return std::numeric_limits<float>::infinity();
		}
		float y = myPoint.y + (-(myNormal.x * (aXPos - myPoint.x) + myNormal.z * (aZPos - myPoint.z)) / myNormal.y);
		return y;
	}

	float Plane::GetZPos(const float aXPos, const float aYPos) const
	{
		if (myNormal.z == 0.f)
		{
			return std::numeric_limits<float>::infinity();
		}
		float z = myPoint.z + (-(myNormal.x * (aXPos - myPoint.x) + myNormal.y * (aYPos - myPoint.y)) / myNormal.z);
		return z;
	}

}
