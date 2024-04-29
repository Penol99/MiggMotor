#include "Ray.h"
#include <DirectXCollision.h>

bool Ray::IntersectsAABB(DirectX::XMVECTOR aBoxCenter, DirectX::XMVECTOR someBoxExtents, float& anOutDistance) const
{
	DirectX::BoundingBox box
	(
		DirectX::XMFLOAT3(DirectX::XMVectorGetX(aBoxCenter), DirectX::XMVectorGetY(aBoxCenter), DirectX::XMVectorGetZ(aBoxCenter)),
		DirectX::XMFLOAT3(DirectX::XMVectorGetX(someBoxExtents), DirectX::XMVectorGetY(someBoxExtents), DirectX::XMVectorGetZ(someBoxExtents))
	);

	return box.Intersects(myOrigin, myDirection, anOutDistance);
}

bool Ray::IntersectsSphere(DirectX::XMVECTOR sphereCenter, float sphereRadius, float& outDistance) const
{
	DirectX::BoundingSphere sphere(DirectX::XMFLOAT3(DirectX::XMVectorGetX(sphereCenter), DirectX::XMVectorGetY(sphereCenter), DirectX::XMVectorGetZ(sphereCenter)), sphereRadius);
	return sphere.Intersects(myOrigin, myDirection,outDistance);
}
