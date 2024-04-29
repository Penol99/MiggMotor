#include "engine_pch.h"
#include "LineSegment2D.h"

namespace mi
{
	LineSegment2D::LineSegment2D(const Vector2f& aStart, const Vector2f& anEnd)
		: myStart(aStart)
		, myEnd(anEnd)
	{
	}

	Vector2f& LineSegment2D::Start()
	{
		return myStart;
	}

	const Vector2f& LineSegment2D::Start() const
	{
		return myStart;
	}

	Vector2f& LineSegment2D::End()
	{
		return myEnd;
	}

	const Vector2f& LineSegment2D::End() const
	{
		return myEnd;
	}

	Vector2f LineSegment2D::GetDirection() const
	{
		return (myEnd - myStart).GetNormalized();
	}

	Vector2f LineSegment2D::GetNormal() const
	{
		const Vector2f direction = GetDirection();
		return Vector2f(-direction.y, direction.x);
	}

	Vector2f LineSegment2D::GetMiddle() const
	{
		return GetScalarPosition(0.5f);
	}

	float LineSegment2D::GetDistance() const
	{
		return Vector2f::Distance(myStart, myEnd);
	}

	Vector2f LineSegment2D::GetScalarPosition(const float aScalar) const
	{
		assert(aScalar >= 0.f && aScalar <= 1.f);
		return myStart + (myEnd - myStart) * aScalar;
	}
}
