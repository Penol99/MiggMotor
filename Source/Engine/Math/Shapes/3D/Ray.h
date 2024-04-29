#pragma once
#include "Vector3.h"

namespace mi
{
	class AABB;
	class Sphere;

	class Ray
	{
	public:
		Ray();
		Ray(const Vector3f& anOrigin, const Vector3f& aDirection);
		Ray(const Ray& aRay);

		const Vector3f& GetOrigin() const;
		void SetOrigin(const Vector3f& anOrigin);

		const Vector3f& GetDirection() const;
		void SetDirection(const Vector3f& aDirection);

		Vector3f GetPointWithScalar(const float aScalar) const;
		Vector3f GetPointWithXPos(const float aXPos) const;
		Vector3f GetPointWithYPos(const float aYPos) const;
		Vector3f GetPointWithZPos(const float aZPos) const;

		bool IntersectsAABB(const AABB& anAABB, float& anOutDistance) const;
		bool IntersectsSphere(const Sphere& aSphere, float& anOutDistance) const;
		bool IntersectsPlane(const DirectX::XMFLOAT3& planePoint, const DirectX::XMFLOAT3& planeNormal, float& hitDistance) const;


	private:
		Vector3f myOrigin;
		Vector3f myDirection;
	};

}
