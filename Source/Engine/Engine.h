#pragma once
#include "WindowContainer.h"
#include "math/vector2.h"

namespace mi
{
	class SoundManager;
	class SpriteDrawer;


	class Engine : WindowContainer
	{
	public:
		bool Init(HINSTANCE hInstance, std::string aWindowTitle, std::string aWindowClass, int aWidth, int aHeight, Timer& aTimer);
		bool ProcessMessages();

		void Update(const float& aDeltaTime);
		void FreeCameraControl(const float& aDeltaTime);
		void Render(std::vector<GameObject*>& aGameObjects);
		void Render();
		void EndFrame();
		void SetActiveCamera(Camera* aCamera);
		ComPtr<ID3D11DeviceContext>& GetDeviceContext();
		ComPtr<ID3D11Device>& GetDevice();
		Timer& GetTimer();
		InputHandler& GetInput();
		Camera& GetCamera();
		SpriteDrawer* GetSpriteDrawer();

		mi::Vector2ui GetRenderResolution();

		ID3D11Buffer* const* GetLightBufferAddress();
		ID3D11DepthStencilState* GetDepthStateAddress();

		static inline Engine* GetInstance()
		{
			if (myInstance == nullptr)
			{
				myInstance = new Engine();
			}
			return myInstance;
		}
		static inline void DestroyInstance()
		{
			delete myInstance;
		}
		
		
	private:
		static Engine* myInstance;

		Engine();
	};
}