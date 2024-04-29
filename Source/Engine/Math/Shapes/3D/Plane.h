#pragma once
#include "Vector3.h"

namespace mi
{
	class Plane
	{
	public:

		Plane() = default;
		Plane(const Vector3f& aPoint, const Vector3f& aNormal);
		~Plane() = default;

		void InitWith3Points(const Vector3f& aPoint1, const Vector3f& aPoint2, const Vector3f& aPoint3);

		void SetPoint(const Vector3f& aPoint);
		void SetNormal(const Vector3f& aNormal);

		const Vector3f& GetPoint() const;
		const Vector3f& GetNormal() const;

		bool IsPointOnNormalSide(const Vector3f& aPoint) const;
		// Is point on the opposite side of the normal
		bool IsPointInside(const Vector3f& aPoint) const;

		float GetXPos(const float aYPos, const float aZPos) const;
		float GetYPos(const float aXPos, const float aZPos) const;
		float GetZPos(const float aXPos, const float aYPos) const;

	private:

		Vector3f myPoint, myNormal;
		
	};
}