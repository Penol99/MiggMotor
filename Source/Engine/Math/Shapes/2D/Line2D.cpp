#include "engine_pch.h"
#include "Line2D.h"

namespace mi
{
	Line2D::Line2D(const Vector2f& aPoint, const Vector2f& aDirection)
		: myPoint(aPoint)
		, myDirection(aDirection.GetNormalized())
	{
	}

	void Line2D::SetPoint(const Vector2f& aPoint)
	{
		myPoint = aPoint;
	}

	void Line2D::SetDirection(const Vector2f& aDirection)
	{
		myDirection = aDirection.GetNormalized();
	}

	const Vector2f& Line2D::GetPoint() const
	{
		return myPoint;
	}

	const Vector2f& Line2D::GetDirection() const
	{
		return myDirection;
	}

	bool Line2D::IsPointOnPositiveSide(const Vector2f& aPoint) const
	{
		const float dot = (aPoint - myPoint).Dot(myDirection.Normal());
		return dot > 0;
	}

	bool Line2D::IsPointOnNegativeSide(const Vector2f& aPoint) const
	{
		return !IsPointOnPositiveSide(aPoint);
	}

	Vector2f Line2D::GetProjectedPoint(const Vector2f& aPoint) const
	{
		const Vector2f v = aPoint - myPoint;
		const float distance = v.Dot(myDirection);
		return myPoint + myDirection * distance;
	}

	float Line2D::GetProjectedTValue(const Vector2f& aPoint) const
	{
		const Vector2f v = aPoint - myPoint;
		return v.Dot(myDirection);
	}

	Vector2f Line2D::GetPointWithXPos(const float anXPos) const
	{
		if (myDirection.x == 0.f)
		{
			return Vector2f(anXPos, 0.f);
		}
		const float yScale = 1.f / myDirection.x;
		const float t = anXPos - myPoint.x;
		const float yPos = myPoint.y + myDirection.y * t * yScale;
		return Vector2f(anXPos, yPos);
	}

	Vector2f Line2D::GetPointWithYPos(const float aYPos) const
	{
		if (myDirection.x == 0.f)
		{
			return Vector2f(0.f, aYPos);
		}
		const float t = aYPos - myPoint.y;
		const float xScale = 1.f / myDirection.y;
		const float xPos = myPoint.x + myDirection.x * t * xScale;
		return Vector2f(xPos, aYPos);
	}

}

