#include "engine_pch.h"
#include "Ray.h"
#include <DirectXCollision.h>
#include "AABB.h"
#include "Sphere.h"

namespace mi
{

	Ray::Ray()
		: myOrigin(Vector3f())
		, myDirection(Vector3f::Forward)
	{
	}

	Ray::Ray(const Vector3f& anOrigin, const Vector3f& aDirection)
		: myOrigin(anOrigin)
		, myDirection(aDirection.GetNormalized())
	{
	}

	Ray::Ray(const Ray& aRay)
		: myOrigin(aRay.myOrigin)
		, myDirection(aRay.myDirection)
	{

	}

	const Vector3f& Ray::GetOrigin() const
	{
		return myOrigin;
	}

	void Ray::SetOrigin(const Vector3f& anOrigin)
	{
		myOrigin = anOrigin;
	}

	const Vector3f& Ray::GetDirection() const
	{
		return myDirection;
	}

	void Ray::SetDirection(const Vector3f& aDirection)
	{
		myDirection = aDirection.GetNormalized();
	}

	Vector3f Ray::GetPointWithScalar(const float aScalar) const
	{
		assert(aScalar >= 0.f && "Scalar of ray can't be negative");
		return myOrigin + myDirection * aScalar;
	}

	Vector3f Ray::GetPointWithXPos(const float aXPos) const
	{
		const float t = (aXPos - myOrigin.x) / myDirection.x;
		const float yPos = myOrigin.y * t * myDirection.y;
		const float zPos = myOrigin.z + t * myDirection.z;
		return Vector3f(aXPos, yPos, zPos);
	}

	Vector3f Ray::GetPointWithYPos(const float aYPos) const
	{
		const float t = (aYPos - myOrigin.y) / myDirection.y;
		const float xPos = myOrigin.x + t * myDirection.x;
		const float zPos = myOrigin.z + t * myDirection.z;
		return Vector3f(xPos, aYPos, zPos);
	}

	Vector3f Ray::GetPointWithZPos(const float aZPos) const
	{
		const float t = (aZPos - myOrigin.z) / myDirection.z;
		const float xPos = myOrigin.x + t * myDirection.x;
		const float yPos = myOrigin.y + t * myDirection.y;
		return Vector3f(xPos, yPos, aZPos);
	}

	bool Ray::IntersectsAABB(const AABB& anAABB, float& anOutDistance) const
	{
		DirectX::BoundingBox box
		(
			anAABB.GetCenter().AsXMFLOAT3(),
			(anAABB.GetSize() / 2.f).AsXMFLOAT3()
		);
		DirectX::XMVECTOR o = myOrigin.AsXMVECTOR();
		DirectX::XMVECTOR d = myDirection.AsXMVECTOR();
		return box.Intersects(o, d, anOutDistance);
	}

	bool Ray::IntersectsSphere(const Sphere& aSphere, float& anOutDistance) const
	{
		DirectX::BoundingSphere sphere(aSphere.myCenter.AsXMFLOAT3(), aSphere.myRadius);
		return sphere.Intersects(myOrigin.AsXMVECTOR(), myDirection.AsXMVECTOR(), anOutDistance);
	}

	bool Ray::IntersectsPlane(const DirectX::XMFLOAT3& planePoint, const DirectX::XMFLOAT3& planeNormal, float& hitDistance) const
	{
		DirectX::XMVECTOR planePt = DirectX::XMLoadFloat3(&planePoint);
		DirectX::XMVECTOR planeNorm = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&planeNormal));

		// Calculate the dot product between ray direction and plane normal
		float dotProduct = DirectX::XMVectorGetX(DirectX::XMVector3Dot(myDirection.AsXMVECTOR(), planeNorm));

		// Check if the ray is nearly parallel to the plane (epsilon is a small value)
		float epsilon = 0.0001f;
		if (fabs(dotProduct) < epsilon)
		{
			// Ray is nearly parallel to the plane
			return false;
		}

		// Calculate the signed distance from the ray origin to the plane
		float signedDistance = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(planePt, myOrigin.AsXMVECTOR()), planeNorm)) / dotProduct;

		// Check if the hit point is in front of the ray (positive distance)
		if (signedDistance >= 0.0f)
		{
			hitDistance = signedDistance;
			return true;
		}

		// No intersection in front of the ray
		return false;
	}
}