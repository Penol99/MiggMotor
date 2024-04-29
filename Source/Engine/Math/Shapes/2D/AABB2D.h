#pragma once
#include "Vector2.h"
#include <vector>

namespace mi
{

	class AABB2D
	{
	public:

		AABB2D() = default;
		AABB2D(const Vector2f& aMin, const Vector2f& aMax);

		void InitWithCenterAndExtents(const Vector2f& aCenter, const Vector2f& someExtents);
		void SetExtents(const Vector2f& someExtents);
		void SetCenter(const Vector2f& aCenter);
		void SetMin(const Vector2f& aMin);
		void SetMax(const Vector2f& aMax);
		void SetMinMax(const Vector2f& aMin, const Vector2f& aMax);

		Vector2f GetCenter() const;
		Vector2f GetExtents() const;
		const Vector2f& GetMin() const;
		const Vector2f& GetMax() const;

		void AdjustSize(const float aSizeMultiplier);
		
	public:

		static AABB2D CreateFromPoints(const std::vector<Vector2f>& somePositions);
		static void UpdateMinMax(Vector2f& aMin, Vector2f& aMax, const Vector2f& aPoint);

	private:

		static void CheckAssert(const Vector2f& aMin, const Vector2f& aMax);

	private:

		Vector2f myMin, myMax;

	};
}