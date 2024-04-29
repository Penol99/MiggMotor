#pragma once
#include <DirectXMath.h>

class Ray
{
public:
    Ray()
        : myOrigin(DirectX::XMVectorSet(0, 0, 0, 0)), myDirection(DirectX::XMVectorSet(0, 0, 1, 0))
    {
    }

    Ray(DirectX::XMVECTOR origin, DirectX::XMVECTOR direction)
        : myOrigin(origin), myDirection(DirectX::XMVector3Normalize(direction))
    {
    }

    Ray(const Ray& ray)
        : myOrigin(ray.myOrigin), myDirection(ray.myDirection)
    {
    }

    DirectX::XMVECTOR GetOrigin() const { return myOrigin; }
    void SetOrigin(DirectX::XMVECTOR newOrigin) { myOrigin = newOrigin; }

    DirectX::XMVECTOR GetDirection() const { return myDirection; }
    void SetDirection(DirectX::XMVECTOR newDirection) { myDirection = DirectX::XMVector3Normalize(newDirection); }

    bool IntersectsAABB(DirectX::XMVECTOR aBoxCenter, DirectX::XMVECTOR someBoxExtents, float& anOutDistance) const;
    bool IntersectsSphere(DirectX::XMVECTOR aSphereCenter, float aSphereRadius, float& anOutDistance) const;


private:
    DirectX::XMVECTOR myOrigin;
    DirectX::XMVECTOR myDirection;
};
