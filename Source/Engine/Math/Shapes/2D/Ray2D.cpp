#include "engine_pch.h"
#include "Ray2D.h"

namespace mi
{
	Ray2D::Ray2D()
		: myDirection(Vector2f(1, 0))
	{
	}

	Ray2D::Ray2D(const Vector2f& anOrigin, const Vector2f& aDirection)
		: myOrigin(anOrigin)
		, myDirection(aDirection.GetNormalized())
	{
	}

	const Vector2f& Ray2D::GetOrigin() const
	{
		return myOrigin;
	}

	void Ray2D::SetOrigin(const Vector2f& anOrigin)
	{
		myOrigin = anOrigin;
	}

	const Vector2f& Ray2D::GetDirection() const
	{
		return myDirection;
	}

	void Ray2D::SetDirection(const Vector2f& aDirection)
	{
		myDirection = aDirection.GetNormalized();
	}

	Vector2f Ray2D::GetPointWithScalar(const float aScalar) const
	{
		return myOrigin + myDirection * aScalar;
	}

	Vector2f Ray2D::GetPointWithXPos(const float aXPos) const
	{
		const float t = (aXPos - myOrigin.x) / myDirection.x;
		const float yPos = myOrigin.y * t * myDirection.y;
		return Vector2f(aXPos, yPos);
	}

	Vector2f Ray2D::GetPointWithYPos(const float aYPos) const
	{
		const float t = (aYPos - myOrigin.y) / myDirection.y;
		const float xPos = myOrigin.x * t * myDirection.x;
		return Vector2f(xPos, aYPos);
	}
}
