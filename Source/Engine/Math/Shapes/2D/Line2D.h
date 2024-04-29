#pragma once
#include "Vector2.h"

namespace mi
{
	class Line2D
	{
	public:
		
		Line2D() = default;
		Line2D(const Vector2f& aPoint, const Vector2f& aDirection);

		void SetPoint(const Vector2f& aPoint);
		void SetDirection(const Vector2f& aDirection);

		const Vector2f& GetPoint() const;
		const Vector2f& GetDirection() const;

		bool IsPointOnPositiveSide(const Vector2f& aPoint) const;
		bool IsPointOnNegativeSide(const Vector2f& aPoint) const;

		Vector2f GetProjectedPoint(const Vector2f& aPoint) const;
		float GetProjectedTValue(const Vector2f& aPoint) const;
	    Vector2f GetPointWithXPos(const float anXPos) const;
		Vector2f GetPointWithYPos(const float aYPos) const;
		
	private:
		Vector2f myPoint, myDirection;
	};
}