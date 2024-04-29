#pragma once
#include "Vector3.h"

namespace mi
{

	class Line
	{
	public:

		Line() = default;
		Line(const Vector3f& aPoint, const Vector3f& aDirection);

		void SetPoint(const Vector3f& aPoint);
		void SetDirection(const Vector3f& aDirection);

		const Vector3f& GetPoint() const;
		const Vector3f& GetDirection() const;

		Vector3f GetProjectedPoint(const Vector3f& aPoint) const;
		float GetProjectedTValue(const Vector3f& aPoint) const;

	private:

		Vector3f myPoint, myDirection;
	};
}