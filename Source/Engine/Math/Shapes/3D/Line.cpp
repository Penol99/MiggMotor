#include "engine_pch.h"
#include "Line.h"

namespace mi
{
	Line::Line(const Vector3f& aPoint, const Vector3f& aDirection)
		: myPoint(aPoint)
		, myDirection(aDirection.GetNormalized())
	{
	}

	void Line::SetPoint(const Vector3f& aPoint)
	{
		myPoint = aPoint;
	}

	void Line::SetDirection(const Vector3f& aDirection)
	{
		myDirection = aDirection.GetNormalized();
	}

	const Vector3f& Line::GetPoint() const
	{
		return myPoint;
	}

	const Vector3f& Line::GetDirection() const
	{
		return myDirection;
	}

	Vector3f Line::GetProjectedPoint(const Vector3f& aPoint) const
	{
		const float t = GetProjectedTValue(aPoint);
		return myPoint + myDirection * t;
	}

	float Line::GetProjectedTValue(const Vector3f& aPoint) const
	{
		const Vector3f v = aPoint - myPoint;
		return myDirection.Dot(v);
	}
}
