#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <PropertyDefines.h>
#include <imgui.h>
#include <json.hpp>
#pragma warning(push)
#pragma warning(disable: 4100)
using json = nlohmann::json;

using namespace DirectX;
namespace mi
{
    class GameObject;
    class Component
    {
    public:
        enum class eComponentType
        {
            Transform,
            Material,
            Model,
            Collider,
            BoxCollider,
            SphereCollider,
            Gizmo
        };

        virtual ~Component() = 0 {};
        virtual void Init() {};
        virtual void Update(const float aDeltaTime = 0.f) {};
        virtual void Render() {};

        const eComponentType GetType() const { return myType; }

        eComponentType myType = eComponentType();
        GameObject* myGameObject = NULL;
    };
}
#pragma warning(pop)

