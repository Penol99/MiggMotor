#include "engine_pch.h"
#include "SpriteDrawer.h"
#include <Engine.h>
#include "Sprite.h"
#include "Sprite3D.h"

#include "Math/FMath.h"
#include <memory>
#include <unordered_map>


namespace mi
{

	SpriteDrawer::SpriteDrawer(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext)
		:myDevice(aDevice), myDeviceContext(aDeviceContext), mySpriteBatch(nullptr), myCommonStates(nullptr)
	{
		Engine& engine = *Engine::GetInstance();

		mySpriteBatch = new DirectX::SpriteBatch(engine.GetDeviceContext().Get());
		myCommonStates = new DirectX::CommonStates(engine.GetDevice().Get());
		DirectX::XMMATRIX orthographicMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)engine.GetRenderResolution().x, (float)engine.GetRenderResolution().y, 0.0f, 0.0f, 1.0f);
		myInverseOrtho = DirectX::XMMatrixInverse(nullptr, orthographicMatrix);
	}

	void SpriteDrawer::Init(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext)
	{
		myDevice = aDevice;
		myDeviceContext = aDeviceContext;
		Engine& engine = *Engine::GetInstance();

		DirectX::XMMATRIX orthographicMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)engine.GetRenderResolution().x, (float)engine.GetRenderResolution().y, 0.0f, 0.0f, 1.0f);
		myInverseOrtho = DirectX::XMMatrixInverse(nullptr, orthographicMatrix);
	}

	SpriteDrawer::~SpriteDrawer()
	{
		delete mySpriteBatch;
		delete myCommonStates;
	}

	void SpriteDrawer::Draw(Sprite* aSprite)
	{
		if (!aSprite->isActive)
		{
			return;
		}
		myDeviceContext->OMSetDepthStencilState(nullptr, 0);

		mySpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, myCommonStates->NonPremultiplied());

		aSprite->myTexture->Bind(0);

		DirectX::XMVECTOR colorVector;
		DirectX::XMFLOAT4 colorFloat4(aSprite->myColor.x, aSprite->myColor.y, aSprite->myColor.z, aSprite->myColor.w);
		colorVector = XMLoadFloat4(&colorFloat4);
		mi::Vector2f pivot = aSprite->myPivot;
		pivot.x *= aSprite->GetPixelSize().x;
		pivot.y *= aSprite->GetPixelSize().y;
		mySpriteBatch->Draw(
			aSprite->myTexture->GetShaderResourceView(), // texture
			DirectX::XMFLOAT2(aSprite->myPosition.x, aSprite->myPosition.y), // position
			&aSprite->myUVRect, // UV RECT
			colorVector, // Color
			aSprite->myRotationDegrees * FMath::DegToRad, // Rotation
			DirectX::XMFLOAT2(pivot.x, pivot.y), // Pivot
			DirectX::XMFLOAT2(aSprite->myScale.x, aSprite->myScale.y), // Scale
			aSprite->myEffect, // SpriteEffect Flip X,Y
			aSprite->myLayerDepth // layerdepth
		);

		mySpriteBatch->End();

	}

	void SpriteDrawer::Draw3D(Sprite3D* aSprite)
	{
		if (!aSprite->isActive)
		{
			return;
		}
		Engine& engine = *Engine::GetInstance();

		Camera* cam = &Engine::GetInstance()->GetCamera();

		DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixLookAtLH(
			aSprite->myPosition.AsXMVECTOR(),
			cam->GetPositionXMVector(),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) // Assuming Y is up
		);
		DirectX::XMMATRIX billboardMatrix = DirectX::XMMatrixInverse(nullptr, lookAtMatrix);



		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(aSprite->myScale.x, aSprite->myScale.y, aSprite->myScale.z);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(aSprite->myPosition.x, aSprite->myPosition.y, aSprite->myPosition.z);

		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(
			aSprite->myRotation.x * FMath::DegToRad,
			aSprite->myRotation.y * FMath::DegToRad,
			aSprite->myRotation.z * FMath::DegToRad
		);

		DirectX::XMMATRIX worldMatrix = scaleMatrix * billboardMatrix * translationMatrix;

		DirectX::XMMATRIX viewMatrix = cam->GetViewMatrix();
		DirectX::XMMATRIX projMatrix = cam->GetProjectionMatrix();

		// The magical formula that fixes everything.
		DirectX::XMMATRIX worldViewProjMatrix = worldMatrix * viewMatrix * projMatrix * myInverseOrtho;

		mySpriteBatch->Begin(
			DirectX::SpriteSortMode_Deferred,
			myCommonStates->NonPremultiplied(),
			nullptr,
			engine.GetDepthStateAddress(),
			nullptr,
			nullptr,
			worldViewProjMatrix
		);

		aSprite->myTexture->Bind(0);
		aSprite->myRotationDegrees = 180.f;

		DirectX::XMVECTOR colorVector;
		DirectX::XMFLOAT4 colorFloat4(aSprite->myColor.x, aSprite->myColor.y, aSprite->myColor.z, aSprite->myColor.w);
		colorVector = XMLoadFloat4(&colorFloat4);
		mi::Vector2f pivot = aSprite->myPivot;
		pivot.x *= aSprite->GetPixelSize().x;
		pivot.y *= aSprite->GetPixelSize().y;
		mySpriteBatch->Draw(
			aSprite->myTexture->GetShaderResourceView(),
			DirectX::XMFLOAT2(aSprite->myPosition.x, aSprite->myPosition.y),
			nullptr,
			colorVector,
			aSprite->myRotationDegrees * FMath::DegToRad,
			DirectX::XMFLOAT2(aSprite->GetPixelSize().x * 0.5f, aSprite->GetPixelSize().y * 0.5f),
			DirectX::XMFLOAT2(aSprite->myScale.x, aSprite->myScale.y),
			aSprite->myEffect,
			aSprite->myLayerDepth
		);

		mySpriteBatch->End();

	}


	void SpriteDrawer::Draw(std::shared_ptr<Sprite>& aSprite)
	{
		Draw(aSprite.get());
	}


	void SpriteDrawer::DrawBatch(std::vector<std::shared_ptr<Sprite>>& someSprites)
	{

		myDeviceContext->OMSetDepthStencilState(nullptr, 0);
		mySpriteBatch->Begin(DirectX::SpriteSortMode_FrontToBack, myCommonStates->NonPremultiplied());
		for (auto& s : someSprites)
		{
			if (!s->isActive)
			{
				continue;
			}

			s->myTexture->Bind(0);
			DirectX::XMVECTOR colorVector;
			DirectX::XMFLOAT4 colorFloat4(s->myColor.x, s->myColor.y, s->myColor.z, s->myColor.w);
			colorVector = XMLoadFloat4(&colorFloat4);
			mi::Vector2f pivot = s->myPivot;

			pivot.x *= s->GetPixelSize().x;
			pivot.y *= s->GetPixelSize().y;
			mySpriteBatch->Draw(
				s->myTexture->GetShaderResourceView(),
				DirectX::XMFLOAT2(s->myPosition.x, s->myPosition.y),
				nullptr,
				colorVector,
				s->myRotationDegrees * FMath::DegToRad,
				DirectX::XMFLOAT2(pivot.x, pivot.y),
				DirectX::XMFLOAT2(s->myScale.x, s->myScale.y),
				s->myEffect,
				s->myLayerDepth
			);
		}

		mySpriteBatch->End();
	}
}