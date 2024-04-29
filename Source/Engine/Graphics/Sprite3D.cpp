#include "engine_pch.h"
#include "Sprite3D.h"
#include "Texture.h"

namespace mi
{

	void Sprite3D::Init(const wchar_t* aPath)
	{
		myTexture = new Texture(Engine::GetInstance()->GetDevice().Get(), Engine::GetInstance()->GetDeviceContext().Get(), aPath);
	}
	Sprite3D::Sprite3D() :
		myPosition(mi::Vector3f(0, 0)),
		myScale(mi::Vector3f(1, 1)),
		myPivot(mi::Vector2f(0, 0)),
		myColor(mi::Vector4f(1, 1, 1, 1)),
		myRotation(mi::Vector3f(0, 0, 0)),
		isActive(true),
		myTexture(nullptr)
	{

	}
	Sprite3D::~Sprite3D()
	{
		delete myTexture;
	}

	mi::Vector2f Sprite3D::GetPixelSize()
	{
		return myTexture->myTextureSize;
	}

	void Sprite3D::ScaleToScreen(const float aRatioOfScreenSize)
	{
		mi::Vector2f screenRes = Engine::GetInstance()->GetRenderResolution().GetAs<float>();
		mi::Vector2f spriteSize = GetPixelSize();
		float scaling{};

		float screenAR = screenRes.X / screenRes.y;
		float spriteAR = spriteSize.X / spriteSize.y;

		if (screenAR > spriteAR)
		{
			// ReSize based on sprite height
			scaling = screenRes.y / spriteSize.y;
		}
		else
		{
			// ReSize based on sprites width
			scaling = screenRes.x / spriteSize.x;
		}
		myScale = { aRatioOfScreenSize * scaling, aRatioOfScreenSize * scaling };
	}

}