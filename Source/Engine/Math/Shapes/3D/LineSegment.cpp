#include "engine_pch.h"
#include "LineSegment.h"

namespace mi
{
	LineSegment::LineSegment(const Vector3f& aStart, const Vector3f& anEnd)
		: myStart(aStart)
		, myEnd(anEnd)
	{
	}

	Vector3f& LineSegment::Start()
	{
		return myStart;
	}

	const Vector3f& LineSegment::Start() const
	{
		return myStart;
	}

	Vector3f& LineSegment::End()
	{
		return myEnd;
	}

	const Vector3f& LineSegment::End() const
	{
		return myEnd;
	}
	Vector3f LineSegment::GetMiddle() const
	{
		return GetScalarPosition(0.5f);
	}
	Vector3f LineSegment::GetScalarPosition(const float aScalar) const
	{
		return myStart + (myEnd - myStart) * aScalar;
	}
}

