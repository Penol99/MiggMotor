#pragma once

#include <Graphics/Mesh.h>
#include "Component.h"
#include "BoneInfo.h"
#include "ModelComponent.h"




namespace mi
{
    struct MaterialData;

    class SkinnedModelComponent : public Component
    {
    public:
        struct SkinnedModelInfo
        {
            std::vector<Mesh> myMeshes;
            std::map<std::string, BoneInfo> myBoneInfoMap;
            int myBoneCounter = 0;
        };
        SkinnedModelComponent();
        ~SkinnedModelComponent() override;
        void Init();
        void Render();
        void RenderImGui() {};

        void SetPixelShader(int aId);
        void SetVertexShader(int aId);
        void SetModelInfo(std::string aPath);
        void AddMaterial(std::string aPath);
        void AddMaterial(int aId);
        void ClearMaterials();

        inline std::map<std::string, BoneInfo>& GetBoneInfoMap() { return myModelInfo->myBoneInfoMap; }
        inline int& GetBoneCount() { return myModelInfo->myBoneCounter; }
        SkinnedModelInfo* myModelInfo;
        std::vector<MaterialData *> myMaterialDatas;
    private:

        ConstantBuffer<CB_ObjectWorldMatrix> myCBObjectMatrix;
        ConstantBuffer<CB_BoneBuffer> myBoneBuffer;
        VertexShader *myVertexShader;
        PixelShader *myPixelShader;
        DirectX::XMMATRIX myBoneTransforms[MAX_ANIMATION_BONES];

        friend class MaterialComponent;
        friend class AnimationPlayerComponent;
        friend class GraphicsPlayground;
    };

}
