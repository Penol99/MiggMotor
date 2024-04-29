#pragma once
#include "Vector3.h"
#include "Line.h"
#include "Ray.h"

namespace mi
{
	class LineSegment
	{
	public:

		LineSegment() = default;
		LineSegment(const Vector3f& aStart, const Vector3f& anEnd);
		~LineSegment() = default;

		Vector3f& Start();
		const Vector3f& Start() const;

		Vector3f& End();
		const Vector3f& End() const;

		Vector3f GetMiddle() const;
		Vector3f GetScalarPosition(const float aScalar) const;

	private:

		Vector3f myStart, myEnd;
	};
}