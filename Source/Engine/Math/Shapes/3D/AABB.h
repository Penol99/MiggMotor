#pragma once
#include "Vector.h"

#include <vector>
#include <array>

namespace mi
{
	class AABB
	{
	public:

		AABB() = default;
		AABB(const Vector3f& aCenter, const Vector3f& aSize);
		~AABB() = default;

		void InitWithCenterAndSize(const Vector3f& aCenter, const Vector3f& aSize);
		void SetSize(const Vector3f& aSize);
		void SetCenter(const Vector3f& aCenter);

		void SetMin(const Vector3f& aMin);
		void SetMax(const Vector3f& aMax);

		Vector3f GetCenter() const;
		Vector3f GetSize() const;
		const Vector3f& GetMin() const;
		const Vector3f& GetMax() const;

	private:

		Vector3f myMin, myMax;

	public:

		static AABB CreateFromPoints(const std::vector<Vector3f>& somePoints);

		template<size_t Size>
		static AABB CreateFromPoints(const std::array<Vector3f, Size>& somePoints);
		static void UpdateMinMax(Vector3f& aMin, Vector3f& aMax, const Vector3f& aPoint);

	private:

		static void CheckAssert(const Vector3f& aMin, const Vector3f& aMax);
	};

	template<size_t Size>
	inline AABB AABB::CreateFromPoints(const std::array<Vector3f, Size>& somePoints)
	{
		Vector3f minPos(FLT_MAX, FLT_MAX);
		Vector3f maxPos(-FLT_MAX, -FLT_MAX);

		for (const Vector3f& point : somePoints)
		{
			AABB::UpdateMinMax(minPos, maxPos, point);
		}
		return AABB(minPos, maxPos);
	}
}