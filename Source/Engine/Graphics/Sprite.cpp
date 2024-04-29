#include "engine_pch.h"
#include "Sprite.h"
#include "Texture.h"

namespace mi
{
    Sprite::Sprite() :
        myPosition(mi::Vector2f(0, 0)),
        myScale(mi::Vector2f(1, 1)),
        myPivot(mi::Vector2f(0, 0)),
        myColor(mi::Vector4f(1, 1, 1, 1)),
        myUVRect({ 0,0,1,1 }),
        isActive(true),
        myTexture(nullptr)
#ifdef _DEBUG
        ,
        myResource(nullptr)
#endif // _DEBUG
        
    {
    }

    void Sprite::Init(const wchar_t* aPath)
    {
        myTexture = new Texture(Engine::GetInstance()->GetDevice().Get(), Engine::GetInstance()->GetDeviceContext().Get(), aPath);
        myUVRect.left = 0;
        myUVRect.top = 0;
        myUVRect.right = (LONG)myTexture->myTextureSize.x;
        myUVRect.bottom = (LONG)myTexture->myTextureSize.y;
    }

    Sprite::~Sprite()
    {
    }

    const Vector2f Sprite::GetPixelSize() const
    {
        return myTexture->myTextureSize;
    }

    void Sprite::ScaleToScreen(const float aRatioOfScreenSize)
    {
        Vector2f screenRes = Engine::GetInstance()->GetRenderResolution().GetAs<float>();
        Vector2f spriteSize = GetPixelSize();
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
