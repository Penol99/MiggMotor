#pragma once
#include "Vector2.h"

namespace mi
{
	class LineSegment2D
	{
	public:

		LineSegment2D() = default;
		LineSegment2D(const Vector2f& aStart, const Vector2f& anEnd);
		~LineSegment2D() = default;

		Vector2f& Start();
		const Vector2f& Start() const;

		Vector2f& End();
		const Vector2f& End() const;

		Vector2f GetDirection() const;
		Vector2f GetNormal() const;

		Vector2f GetMiddle() const;

		float GetDistance() const;
		Vector2f GetScalarPosition(const float aScalar) const;

	private:

		Vector2f myStart, myEnd;
	};
}