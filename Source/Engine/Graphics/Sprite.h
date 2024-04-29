#pragma once
#include <Engine.h>
#include <Math/Vector.h>
#include <utility>

namespace mi
{
	class Texture;
	class SpriteResource;
	class Rectangle;
	class UI;

	class Sprite
	{
	public:
		void Init(const wchar_t* aPath);
		Sprite();
		~Sprite();

		bool isActive;
		Vector2f myPosition;
		Vector2f myScale;
		Vector2f myPivot;
		//mi::Vector2f myUVStart;
		size_t myFrameIndex;
		RECT myUVRect; // LONG left, LONG top, LONG right, LONG bottom;
		mi::Vector4f myColor;
		DirectX::SpriteEffects myEffect = DirectX::SpriteEffects_None;
		float myLayerDepth = 0.0f;
		float myRotationDegrees = 0.0f;

		const Vector2f GetPixelSize() const;
		void ScaleToScreen(const float aRatioOfScreenSize = 1.0f);
	private:
		friend class mi::UI;
		friend class SpriteDrawer;
		friend class mi::SpriteResource;
		Texture* myTexture;
#ifdef _DEBUG
		SpriteResource* myResource;
#endif // _DEBUG

	};
}
