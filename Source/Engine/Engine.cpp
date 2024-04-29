#include "Engine.h"
#include <iostream>
#include "Timer.h"






namespace mi
{
	Engine* Engine::myInstance = nullptr;
	Engine::Engine()
	{
	}

	bool Engine::Init(HINSTANCE hInstance, std::string aWindowTitle, std::string aWindowClass, int aWidth, int aHeight, Timer& aTimer)
	{
		if (!this->myRenderWindow.Init(myGfx, this, hInstance, aWindowTitle, aWindowClass, aWidth, aHeight))
		{
			return false;
		}
		if (!myGfx.Init(myRenderWindow.GetHWND(), aWidth * (int)myRenderWindow.GetScaleFactor(), aHeight * (int)myRenderWindow.GetScaleFactor(), aTimer))
		{
			return false;
		}

		myGfx;
		ImGui::GetIO().FontGlobalScale = myRenderWindow.GetScaleFactor();
		myInput.SetHWND(myRenderWindow.GetHWND());
		return true;
	}

	bool Engine::ProcessMessages()
	{
		return this->myRenderWindow.ProcessMessages();
	}

	void Engine::Update(const float& aDeltaTime)
	{
		aDeltaTime;
		myInput.Update();
		FreeCameraControl(aDeltaTime);
	}

	void Engine::FreeCameraControl(const float& aDeltaTime)
	{
		Ray test;
		if (myInput.IsMouseButtonDown(VK_RBUTTON))
		{
			const float camRotSpeed = 2.f * aDeltaTime;
			myGfx.myCurrentCamera->AdjustRotation({ (float)myInput.GetMouseDelta().y * camRotSpeed, (float)myInput.GetMouseDelta().x * camRotSpeed, 0.f });
			float camSpeed = myGfx.myCameraSpeed * aDeltaTime;
			if (myInput.IsKeyDown(VK_SHIFT))
			{
				camSpeed *= 2;
			}
			if (myInput.IsKeyDown(VK_KEY_W))
			{
				myGfx.myCurrentCamera->AdjustPosition(myGfx.myCurrentCamera->GetForward() * camSpeed);
			}
			if (myInput.IsKeyDown(VK_KEY_S))
			{
				myGfx.myCurrentCamera->AdjustPosition(myGfx.myCurrentCamera->GetBack() * camSpeed);
			}
			if (myInput.IsKeyDown(VK_KEY_D))
			{
				myGfx.myCurrentCamera->AdjustPosition(myGfx.myCurrentCamera->GetRight() * camSpeed);
			}
			if (myInput.IsKeyDown(VK_KEY_A))
			{
				myGfx.myCurrentCamera->AdjustPosition(myGfx.myCurrentCamera->GetLeft() * camSpeed);
			}
			if (myInput.IsKeyDown(VK_SPACE))
			{
				myGfx.myCurrentCamera->AdjustPosition({ 0, camSpeed, 0.f });
			}
			if (myInput.IsKeyDown(VK_CONTROL))
			{
				myGfx.myCurrentCamera->AdjustPosition({ 0, -camSpeed, 0.f });
			}
		}
	}

	void Engine::Render(std::vector<GameObject*>& aGameObjects)
	{
		myGfx.Render3D(aGameObjects);

	}
	void Engine::Render()
	{
		myGfx.Render2D();

	}

	void Engine::EndFrame()
	{
		myGfx.mySwapChain->Present(1,0);
	}

	void Engine::SetActiveCamera(Camera* aCamera)
	{
		myGfx.myCurrentCamera = aCamera;
	}

	ComPtr<ID3D11DeviceContext>& Engine::GetDeviceContext()
	{
		return myGfx.myDeviceContext;
	}

	ComPtr<ID3D11Device>& Engine::GetDevice()
	{
		return myGfx.myDevice;
	}

	Timer& Engine::GetTimer()
	{
		return *myGfx.myTimer;
	}

	InputHandler& Engine::GetInput()
	{
		return myInput;
	}

	Camera& Engine::GetCamera()
	{
		return *myGfx.myCurrentCamera;
	}

	SpriteDrawer* Engine::GetSpriteDrawer()
	{
		return myGfx.mySpriteDrawer;
	}

	mi::Vector2ui Engine::GetRenderResolution()
	{
		return myRenderWindow.GetScreenSize();
	}

	ID3D11Buffer* const* Engine::GetLightBufferAddress()
	{
		return myGfx.myCBLight.GetAddressOf();
	}

	ID3D11DepthStencilState* Engine::GetDepthStateAddress()
	{
		return myGfx.myDepthStencilState.Get();
	}

	
}
