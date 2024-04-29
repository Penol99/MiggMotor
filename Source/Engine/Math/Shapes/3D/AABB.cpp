#include "engine_pch.h"
#include "Math/Shapes/3D/AABB.h"
#include "FMath.h"
#include "AABB2D.h"

namespace mi
{
	AABB::AABB(const Vector3f& aCenter, const Vector3f& aSize)
	{
		InitWithCenterAndSize(aCenter, aSize);
		CheckAssert(myMin, myMax);
	}

	void AABB::InitWithCenterAndSize(const Vector3f& aCenter, const Vector3f& aSize)
	{
		myMin = aCenter - aSize / 2.f;
		myMax = myMin + aSize;
		CheckAssert(myMin, myMax);
	}

	void AABB::SetSize(const Vector3f& aSize)
	{
		InitWithCenterAndSize(GetCenter(), aSize);
	}

	void AABB::SetCenter(const Vector3f& aCenter)
	{
		Vector3f centerDiff = aCenter - GetCenter();
		myMin += centerDiff;
		myMax += centerDiff;
	}

	void AABB::SetMin(const Vector3f& aMin)
	{
		CheckAssert(aMin, myMax);
		myMin = aMin;
	}

	void AABB::SetMax(const Vector3f& aMax)
	{
		CheckAssert(myMin, aMax);
		myMax = aMax;
	}

	Vector3f AABB::GetCenter() const
	{
		return FMath::Lerp(myMin, myMax, 0.5f);
	}

	Vector3f AABB::GetSize() const
	{
		return myMax - myMin;
	}

	const Vector3f& AABB::GetMin() const
	{
		return myMin;
	}

	const Vector3f& AABB::GetMax() const
	{
		return myMax;
	}

	AABB AABB::CreateFromPoints(const std::vector<Vector3f>& somePoints)
	{
		Vector3f minPos(FLT_MAX, FLT_MAX);
		Vector3f maxPos(-FLT_MAX, -FLT_MAX);

		for (const Vector3f& point : somePoints)
		{
			AABB::UpdateMinMax(minPos, maxPos, point);
		}
		return AABB(minPos, maxPos);
	}

	void AABB::UpdateMinMax(Vector3f& aMin, Vector3f& aMax, const Vector3f& aPoint)
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
		if (aPoint.z > aMax.z)
		{
			aMax.z = aPoint.z;
		}
		if (aPoint.z < aMin.z)
		{
			aMin.z = aPoint.z;
		}
	}

	void AABB::CheckAssert(const Vector3f& aMin, const Vector3f& aMax)
	{
		aMin; aMax;
		assert(aMin.x <= aMax.x && aMin.y <= aMax.y && aMin.z <= aMax.z);
	}


}
