#pragma once

#include <Graphics/Mesh.h>
#include "Component.h"
#include <string>
#include <map>
#include <DirectXMath.h>
#include <memory>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include <ConstantBuffer.h>

namespace mi
{
	struct MaterialData;
	class VFXMaterialComponent;
	struct ModelInfo
	{
		std::vector<Mesh> myMeshes;
	};
	class ModelComponent : public Component
	{
	public:

		ModelComponent();
		~ModelComponent() override;
		void Init();
		void Render();
		
		void SetPixelShader(int aId);
		void SetVertexShader(int aId);
		void SetModelInfo(std::string aPath);
		void AddMaterial(std::string aPath);
		void AddMaterial(int aId);


		inline static DirectX::XMMATRIX ConvertAiToXM(const aiMatrix4x4& aiMat) {
			DirectX::XMMATRIX xmMat = DirectX::XMMATRIX(
				aiMat.a1, aiMat.a2, aiMat.a3, aiMat.a4,
				aiMat.b1, aiMat.b2, aiMat.b3, aiMat.b4,
				aiMat.c1, aiMat.c2, aiMat.c3, aiMat.c4,
				aiMat.d1, aiMat.d2, aiMat.d3, aiMat.d4
			);
			return xmMat;
		}

		void RenderImGui() {};
		ModelInfo *myModelInfo;
	private:

		ConstantBuffer<CB_ObjectWorldMatrix> myCBObjectMatrix;
		VertexShader *myVertexShader;
		PixelShader *myPixelShader;
	
		friend class MaterialComponent;
	};

}
