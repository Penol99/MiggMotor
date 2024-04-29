#pragma once
#include <Engine.h>
#include <Math/Vector.h>



namespace mi
{
	class Texture;

	class Sprite3D
	{
	public:
		void Init(const wchar_t* aPath);
		Sprite3D();
		~Sprite3D();

		bool isActive;
		mi::Vector3f myPosition;
		mi::Vector3f myScale;
		mi::Vector2f myPivot;
		mi::Vector4f myColor;
		mi::Vector3f myRotation;
		DirectX::SpriteEffects myEffect = DirectX::SpriteEffects_None;
		float myLayerDepth = 0.0f;
		float myRotationDegrees = 0.0f;
		mi::Vector2f GetPixelSize();

		void ScaleToScreen(const float aRatioOfScreenSize = 1.0f);
	private:
		Texture* myTexture;

		friend class SpriteDrawer;
	};

}

