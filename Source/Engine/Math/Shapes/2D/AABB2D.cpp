#include "engine_pch.h"
#include "Math/Shapes/3D/AABB.h"
#include "FMath.h"
#include "AABB2D.h"

namespace mi
{
	AABB2D::AABB2D(const Vector2f& aMin, const Vector2f& aMax)
		: myMin(aMin)
		, myMax(aMax)
	{
		AABB2D::CheckAssert(myMin, myMax);
	}

	void AABB2D::InitWithCenterAndExtents(const Vector2f& aCenter, const Vector2f& someExtents)
	{
		myMin = aCenter - someExtents / 2.f;
		myMax = myMin + someExtents;
	}

	void AABB2D::SetExtents(const Vector2f& someExtents)
	{
		InitWithCenterAndExtents(GetCenter(), someExtents);
	}

	void AABB2D::SetCenter(const Vector2f& aCenter)
	{
		Vector2f centerDiff = aCenter - GetCenter();
		myMin += centerDiff;
		myMax += centerDiff;
	}

	Vector2f AABB2D::GetCenter() const
	{
		return FMath::Lerp(myMin, myMax, 0.5f);
	}

	Vector2f AABB2D::GetExtents() const
	{
		return myMax - myMin;
	}

	const Vector2f& AABB2D::GetMin() const
	{
		return myMin;
	}

	const Vector2f& AABB2D::GetMax() const
	{
		return myMax;
	}

	void AABB2D::AdjustSize(const float aSizeMultiplier)
	{
		const Vector2f center = GetCenter();
		const Vector2f minToCenter = GetMin() - center;
		const Vector2f maxToCenter = GetMax() - center;
		SetMin(center + minToCenter * aSizeMultiplier);
		SetMax(center + maxToCenter * aSizeMultiplier);
	}

	void AABB2D::SetMin(const Vector2f& aMin)
	{
		CheckAssert(aMin, myMax);
		myMin = aMin;
	}

	void AABB2D::SetMax(const Vector2f& aMax)
	{
		CheckAssert(myMin, aMax);
		myMax = aMax;
	}

	void AABB2D::SetMinMax(const Vector2f& aMin, const Vector2f& aMax)
	{
		CheckAssert(aMin, aMax);
		myMin = aMin;
		myMax = aMax;
	}

	AABB2D AABB2D::CreateFromPoints(const std::vector<Vector2f>& somePoints)
	{
		Vector2f minPos(FLT_MAX, FLT_MAX);
		Vector2f maxPos(-FLT_MAX, -FLT_MAX);

		for (auto& point : somePoints)
		{
			AABB2D::UpdateMinMax(minPos, maxPos, point);
		}
		return AABB2D(minPos, maxPos);
	}

	void AABB2D::UpdateMinMax(Vector2f& aMin, Vector2f& aMax, const Vector2f& aPoint)
	{
		if (aPoint.x > aMax.x)
		{
			aMax.x = aPoint.x;
		}
		if (aPoint.x < aMin.x)
		{
			aMin.x = aPoint.x;
		}
		if (aPoint.y > aMax.y)
		{
			aMax.y = aPoint.y;
		}
		if (aPoint.y < aMin.y)
		{
			aMin.y = aPoint.y;
		}
	}

	void AABB2D::CheckAssert(const Vector2f& aMin, const Vector2f& aMax)
	{
		aMin; aMax;
		assert(aMin.x <= aMax.x && aMin.y <= aMax.y);
	}
}
