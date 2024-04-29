#pragma once
#include <Math/Vector2.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include <vector>
#include "Texture.h"



namespace mi
{
	class Sprite;
	class Sprite3D;

	class SpriteResource;


	class SpriteDrawer
	{
	public:
		SpriteDrawer(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext);
		~SpriteDrawer();
		void Draw(std::shared_ptr<Sprite>& aSprite);
		void Draw3D(Sprite3D* aSprite);
		void DrawBatch(std::vector<std::shared_ptr<Sprite>>& someSprites);
		void DrawBatch3D(std::vector<Sprite3D*>& someSprites);
		void Draw(Sprite* aSprite);
	private:
		void Init(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext);
		ID3D11Device* myDevice;
		ID3D11DeviceContext* myDeviceContext;

		DirectX::SpriteBatch* mySpriteBatch;
		DirectX::CommonStates* myCommonStates;
		DirectX::XMMATRIX myInverseOrtho;


		friend class Graphics;
	};

}

