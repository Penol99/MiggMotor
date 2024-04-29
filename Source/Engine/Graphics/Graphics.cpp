#include "engine_pch.h"
#define _USE_MATH_DEFINES
#include <Graphics/Light.h>
#include <Graphics.h>
#include <SpriteDrawer.h>
#include <Engine.h>
#include <GameObject.h>
#include <FMath.h>
#include <Animation.h>
#include <Bone.h>
#include <Sprite3D.h>
#include "TerrainGenerator.h"
#define DEPTH_MAP_VIEWPORT_SCALE 6

namespace mi
{

	Graphics::~Graphics()
	{

		delete my3DSprite;
		delete mySpriteDrawer;
		delete myCurrentCamera;
		mySwapChain->Release();
		myDevice->Release();
		myDeviceContext->Release();

	}

	bool Graphics::Init(HWND hwnd, int aWidth, int aHeight, Timer& aTimer)
	{

		myTimer = &aTimer;
		myWidth = aWidth;
		myHeight = aHeight;
		myHandle = hwnd;


		if (!InitDirectX(hwnd))
		{
			return false;
		}
		if (!InitScene())
		{
			return false;
		}
		if (!InitGrid())
		{
			return false;
		}
		if (!InitSkybox())
		{
			return false;
		}

		InitializeDeferredBuffers();

		mySpriteDrawer = new SpriteDrawer(myDevice.Get(), myDeviceContext.Get());


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(myDevice.Get(), myDeviceContext.Get());
		ImGui::StyleColorsDark();

		return true;
	}


	void Graphics::SetCubemapTexture(const std::wstring& cubemapDDSFilePath)
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(myDevice.Get(), cubemapDDSFilePath.c_str(), nullptr, myEnvironmentTexture.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to load cubemap texture from file");
		}
	}

	void Graphics::Render2D()
	{
		static std::vector<mi::GameObject*> tempObjects;
		Render3D(tempObjects);
	}



	void Graphics::InitializeDeferredBuffers() {
		myRTPosition.Init(myWidth, myHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
		myRTNormal.Init(myWidth, myHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
		myRTAlbedo.Init(myWidth, myHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		myRTInputNormals.Init(myWidth, myHeight);
		myRTMaterial.Init(myWidth, myHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
		myRTLighting.Init(myWidth, myHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
		myRTDirectionalLightPos.Init(myWidth, myHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	}

	void Graphics::RenderSceneToGBuffers(std::vector<mi::GameObject*>& aGameObjects) {


		ID3D11RenderTargetView* renderTargets[] = {
		myRTPosition.GetRenderTargetView().Get(),
		myRTNormal.GetRenderTargetView().Get(),
		myRTAlbedo.GetRenderTargetView().Get(),
		myRTInputNormals.GetRenderTargetView().Get(),
		myRTMaterial.GetRenderTargetView().Get(),
		myRTDirectionalLightPos.GetRenderTargetView().Get(),
		};

		myDeviceContext->OMSetRenderTargets(ARRAYSIZE(renderTargets), renderTargets, myDepthStencilTarget.GetDepthResourceView().Get());
		myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 1);
		myDeviceContext->ClearDepthStencilView(myDepthStencilTarget.GetDepthResourceView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		myDeviceContext->ClearRenderTargetView(myRTPosition.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearRenderTargetView(myRTNormal.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearRenderTargetView(myRTAlbedo.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearRenderTargetView(myRTInputNormals.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearRenderTargetView(myRTMaterial.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearRenderTargetView(myRTDirectionalLightPos.GetRenderTargetView().Get(), myBackgroundColor);



		myTransparentObjects.clear();
		myTransparentObjects.shrink_to_fit();
		for (GameObject* gameObject : aGameObjects)
		{
			//	if (gameObject->IsTransparent())
			//	{
			//		myTransparentObjects.push_back(gameObject);
			//	}
			//	else
			{
				gameObject->Render();
			}

		}


	}

	void Graphics::PerformLightingPass() {
		// Bind the G-buffer textures as shader resources
		ID3D11ShaderResourceView* gBufferSRVs[] = {
			myRTPosition.GetShaderResourceView().Get(),
			myRTNormal.GetShaderResourceView().Get(),
			myRTAlbedo.GetShaderResourceView().Get(),
			myRTInputNormals.GetShaderResourceView().Get(),
			myRTMaterial.GetShaderResourceView().Get(),
			myRTDirectionalLightPos.GetShaderResourceView().Get()
		};


		myDeviceContext->OMSetRenderTargets(1, myRTLighting.GetRenderTargetView().GetAddressOf(), nullptr);
		myDeviceContext->ClearRenderTargetView(myRTLighting.GetRenderTargetView().Get(), myBackgroundColor);

		myDeviceContext->PSSetShaderResources(5, ARRAYSIZE(gBufferSRVs), gBufferSRVs);


		myDeviceContext->PSSetShader(myLightingPixelShader.GetShader(), nullptr, 0);

		RenderFullScreenQuad();

	}



	void Graphics::RenderShadowPass(std::vector<mi::GameObject*>& someGameObjects)
	{
		myDeviceContext->RSSetViewports(1, &myDepthViewport);

		Camera* originalCamera = myCurrentCamera;

		DirectX::XMVECTOR lightDirection = DirectX::XMVectorSet(
			Light::DirectionalLight::myDirectionalLightDirection.x,
			Light::DirectionalLight::myDirectionalLightDirection.y,
			Light::DirectionalLight::myDirectionalLightDirection.z, 0.0f);


		static float distance = 10.f;

		//ImGui::Begin("Directional Light Depth Camera");
		//ImGui::DragFloat("Distance", &distance);
		//ImGui::End();

		DirectX::XMVECTOR lightPosition = DirectX::XMVectorScale(lightDirection, distance);

		Vector3f lookAtPoint = { 0.0f, 0.0f, 0.0f };//originalCamera.GetPosition();;

		myDirectionalLightCamera.SetXMVectorPosition(lightPosition);
		myDirectionalLightCamera.SetLookAtPos(XMFLOAT3(lookAtPoint.x, lookAtPoint.y, lookAtPoint.z));

		//float orthoWidth = 100.0f;
		//float orthoHeight = 100.0f;
		//float nearPlane = 1.1f;
		//float farPlane = 10.f;
		static float sceneRadius = 600;

		ImGui::Begin("Directional Light Depth Camera");
		ImGui::DragFloat("sceneRadius", &sceneRadius);
		ImGui::End();
		ImGui::Begin("Directional Light Depth Camera");
		ImGui::DragFloat("bias", &myCBShadowLightBuffer.myData.shadowBias, 0.001f);
		ImGui::End();
		float orthoWidth = sceneRadius;
		float orthoHeight = sceneRadius;


		float maxSceneDistance = distance;


		float nearPlane = (maxSceneDistance - sceneRadius);
		nearPlane;
		float farPlane = maxSceneDistance + sceneRadius;
		farPlane;
		myDirectionalLightCamera.SetOrthographicProjection(orthoWidth, orthoHeight, 100.f, 1000.f);

		myCurrentCamera = &myDirectionalLightCamera;

		UpdateConstantBuffers();

		myDeviceContext->OMSetRenderTargets(0, nullptr, myDepthStencilTarget.GetDepthResourceView().Get());
		myDeviceContext->ClearDepthStencilView(myDepthStencilTarget.GetDepthResourceView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 1);

		myDeviceContext->RSSetState(myShadowRasterizerState.Get());
		// Render the scene from the light's perspective
		myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);

		// Render game objects
		for (auto& gameObject : someGameObjects)
		{
			/*if (gameObject->IsTransparent())
			{
				continue;
			}*/
			gameObject->Render();
		}
		//myShadowRasterizerState->Release();




		myDeviceContext->OMSetRenderTargets(1, myShadowMapRenderTarget.GetRenderTargetView().GetAddressOf(), nullptr);
		myDeviceContext->ClearRenderTargetView(myShadowMapRenderTarget.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->PSSetShader(myDepthTargetPixelShader.GetShader(), nullptr, 0);
		// Set Which render target to be displayed on the screen(sent to the backbuffer)
		myDeviceContext->PSSetShaderResources(1, 1, myDepthStencilTarget.GetShaderResourceView().GetAddressOf());
		myDeviceContext->RSSetState(myRasterizerState.Get());

		RenderFullScreenQuad();
		myCurrentCamera = originalCamera;

		ImGui::Begin("Orthographic Depth View");
		ImGui::Image(myShadowMapRenderTarget.GetShaderResourceView().Get(), ImVec2(480, 270));
		ImGui::End();


#ifdef GRAPHICS_TESTING


#endif // _DEBUG
	}

	void Graphics::Render3D(std::vector<mi::GameObject*>& aGameObjects)
	{
		UNREFERENCED_PARAMETER(aGameObjects);
		myDeviceContext->RSSetViewports(1, &mySceneViewport);
		myDeviceContext->RSSetState(myRasterizerState.Get());


		RenderLightPropertyWindows();



		ID3D11RenderTargetView* oldRenderTarget;
		ID3D11DepthStencilView* oldDepthStencilView;
		myDeviceContext->OMGetRenderTargets(1, &oldRenderTarget, &oldDepthStencilView);





		myDeviceContext->OMSetRenderTargets(1, myRTDepthCamera.GetRenderTargetView().GetAddressOf(), myDepthStencilTarget.GetDepthResourceView().Get());
		myDeviceContext->ClearRenderTargetView(myRTDepthCamera.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearDepthStencilView(myDepthStencilTarget.GetDepthResourceView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		myDeviceContext->PSSetShader(myDepthTargetPixelShader.GetShader(), nullptr, 0);

		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		myDeviceContext->OMSetDepthStencilState(myTransDepthStencilState.Get(), 1);
		myDeviceContext->OMSetBlendState(myBlendState.Get(), blendFactor, 0xffffffff);

		// Render your objects
		for (auto& g : aGameObjects)
		{
			g->Render();
		}
		myDeviceContext->OMSetRenderTargets(1, myRTDepthCamera.GetRenderTargetView().GetAddressOf(), nullptr);
		myDeviceContext->PSSetShader(myDepthTargetPixelShader.GetShader(), nullptr, 0);
		myDeviceContext->PSSetShaderResources(1, 1, myDepthStencilTarget.GetShaderResourceView().GetAddressOf());


		RenderFullScreenQuad();

		myDeviceContext->OMSetRenderTargets(1, myTerrainSceneRT.GetRenderTargetView().GetAddressOf(), myDepthStencilTarget.GetDepthResourceView().Get());
		myDeviceContext->ClearRenderTargetView(myTerrainSceneRT.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearDepthStencilView(myDepthStencilTarget.GetDepthResourceView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		Vector3f waterPlanePoint(0.0f, 0.f, 0.0f);
		Vector3f waterPlaneNormal(0.0f, 1.0f, 0.0f);

		Vector3f cameraPosition = myCurrentCamera->GetPosition();
		Vector3f mirroredPosition = cameraPosition - 2.0f * waterPlaneNormal * (cameraPosition.y - waterPlanePoint.y);

		myReflectionCamera->SetPosition(mirroredPosition);

		Vector3f reflectionRotation = myCurrentCamera->GetRotation();
		reflectionRotation.x = -reflectionRotation.x;
		myReflectionCamera->SetRotation(reflectionRotation);

		myReflectionCamera->UpdateViewMatrix();

		myCurrentCamera = myReflectionCamera;
		UpdateConstantBuffers();
		myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 1);

		myTerrainGenerator->Render(myDeviceContext.Get());


		myDeviceContext->OMSetRenderTargets(1, mySceneRT.GetRenderTargetView().GetAddressOf(), myDepthStencilTarget.GetDepthResourceView().Get());
		myDeviceContext->ClearRenderTargetView(mySceneRT.GetRenderTargetView().Get(), myBackgroundColor);
		myDeviceContext->ClearDepthStencilView(myDepthStencilTarget.GetDepthResourceView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		ID3D11ShaderResourceView* nullSRV[] = { nullptr };
		myDeviceContext->PSSetShaderResources(1, 1, nullSRV);

#ifdef DISPLAYGRID
		RenderGrid();
#endif 


		myCurrentCamera = myMainCamera;

		UpdateLightBuffer();
		UpdateConstantBuffers();
		if (myEnvironmentTexture != NULL)
		{
			Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, myEnvironmentTexture.GetAddressOf());
		}
		myDeviceContext->OMSetDepthStencilState(mySkyboxDepthStencilState.Get(), 1);
		myDeviceContext->RSSetState(mySkyboxRasterizer.Get());
		RenderSkybox();

		myDeviceContext->PSSetShaderResources(14, 1, myRTDepthCamera.GetShaderResourceView().GetAddressOf());

		myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 1);
		myDeviceContext->RSSetState(myRasterizerState.Get());

		myTerrainGenerator->Render(myDeviceContext.Get());
		myDeviceContext->PSSetShaderResources(90, 1, myTerrainSceneRT.GetShaderResourceView().GetAddressOf());

		for (auto& g : aGameObjects)
		{
			g->Render();
		}

		myDeviceContext->PSSetShaderResources(14, 1, nullSRV);

		ExtractBrightAreas();

		BlurBloomTexture();

		CombineBloomWithScene();


		// Set rendertarget to the backbuffers rendertarget(makes everything visible on the screen)
		myDeviceContext->OMSetRenderTargets(1, myBackBufferRenderTargetView.GetAddressOf(), nullptr);
		myDeviceContext->PSSetShader(myRenderTargetPixelShader.GetShader(), nullptr, 0);
		// Set Which render target to be displayed on the screen(sent to the backbuffer)
		myDeviceContext->PSSetShaderResources(1, 1, myBloomSceneRT.GetShaderResourceView().GetAddressOf());
		// Renders a quad the size of the screen with the texture of the bloomscene.

		RenderFullScreenQuad();


		myDeviceContext->PSSetShaderResources(1, 1, nullSRV);
		RestoreRenderTargetsAndStates(oldRenderTarget, oldDepthStencilView);
		//		UpdateLightBuffer();
		//
		//		ID3D11RenderTargetView* oldRenderTarget;
		//		ID3D11DepthStencilView* oldDepthStencilView;
		//
		//		myDeviceContext->OMGetRenderTargets(1, &oldRenderTarget, &oldDepthStencilView);
		//
		//		RenderShadowPass(aGameObjects);
		//
		//		myDeviceContext->RSSetViewports(1, &mySceneViewport);
		//		myDeviceContext->RSSetState(myRasterizerState.Get());
		//
		//		UpdateConstantBuffers();
		//
		//		RenderSceneToGBuffers(aGameObjects);
		//		myDeviceContext->PSSetShaderResources(4, 1, myShadowMapRenderTarget.GetShaderResourceView().GetAddressOf());
		//		if (myEnvironmentTexture != NULL)
		//		{
		//			Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, myEnvironmentTexture.GetAddressOf());
		//		}
		//		UpdateConstantBuffers();
		//		UpdateLightBuffer();
		//		PerformLightingPass();
		//
		//
		//		myDeviceContext->OMSetRenderTargets(1, myRTLighting.GetRenderTargetView().GetAddressOf(), nullptr);
		//		myDeviceContext->ClearRenderTargetView(myRTLighting.GetRenderTargetView().Get(), myBackgroundColor);
		//#ifdef DISPLAYGRID
		//		RenderGrid();
		//#endif 
		//
		//		SortTransparentObjects(myTransparentObjects);
		//		RenderTransparentObjects(myTransparentObjects);
		//
		//
		//		//Engine::GetInstance()->GetDebugDrawer()->Render();
		//
		//		//Render3DSprites();
		//
		//		//TODO: FIX WEIRD BUG WHERE BLOOM IS CLONING EVERY MESH ON EVERY SIDE WHEN CAMERA IS PASSING BY ORIGO HORIZONTALLY
		//
		//		//ExtractBrightAreas();
		//		//
		//		//BlurBloomTexture();
		//		//
		//		//CombineBloomWithScene();
		//
		//
		//		ID3D11ShaderResourceView* gBufferSRVs[] = {
		//			myRTLighting.GetShaderResourceView().Get(),
		//			myBloomSceneRT.GetShaderResourceView().Get(),
		//			myRTPosition.GetShaderResourceView().Get(),
		//			myRTNormal.GetShaderResourceView().Get(),
		//			myRTAlbedo.GetShaderResourceView().Get(),
		//			myRTInputNormals.GetShaderResourceView().Get(),
		//			myRTMaterial.GetShaderResourceView().Get(),
		//			myRTDirectionalLightPos.GetShaderResourceView().Get()
		//		};
		//
		//		if (Engine::GetInstance()->GetInput().IsKeyPressed(VK_F6))
		//		{
		//			myCurrentRenderPass = myCurrentRenderPass < (int)ARRAYSIZE(gBufferSRVs) - 1 ? myCurrentRenderPass + 1 : 0;
		//		}
		//		ID3D11ShaderResourceView* current = gBufferSRVs[myCurrentRenderPass];
		//
		//		// Set rendertarget to the backbuffers rendertarget(makes everything visible on the screen)
		//		myDeviceContext->OMSetRenderTargets(1, myBackBufferRenderTargetView.GetAddressOf(), nullptr);
		//		myDeviceContext->PSSetShader(myRenderTargetPixelShader.GetShader(), nullptr, 0);
		//		// Set Which render target to be displayed on the screen(sent to the backbuffer)
		//		myDeviceContext->PSSetShaderResources(1, 1, &current);
		//		// Renders a quad the size of the screen with the texture of the bloomscene.
		//
		//		RenderFullScreenQuad();
		//		//mySceneRT = myRTLighting;
		//
		//		RenderSpritesAndDisplayImages();
		//#ifdef GRAPHICS_TESTING
		//		RenderLightPropertyWindows();
		//#endif
		//
		//
		//		RestoreRenderTargetsAndStates(oldRenderTarget, oldDepthStencilView);

	}

	void Graphics::Render3DSprites()
	{
		ImGui::Begin("3D Sprite");

		ImGui::Text("Position");
		ImGui::DragFloat3("##Position", reinterpret_cast<float*>(&my3DSprite->myPosition), 0.1f);

		// Edit scale using DragFloat3
		ImGui::Text("Scale");
		ImGui::DragFloat3("##Scale", reinterpret_cast<float*>(&my3DSprite->myScale), 0.1f);

		// Edit pivot using DragFloat2
		ImGui::Text("Pivot");
		ImGui::DragFloat2("##Pivot", reinterpret_cast<float*>(&my3DSprite->myPivot), 0.1f);

		// Edit color using ColorEdit4 (RGBA)
		ImGui::Text("Color");
		ImGui::ColorEdit4("##Color", reinterpret_cast<float*>(&my3DSprite->myColor));

		// Edit rotation using DragFloat3
		ImGui::Text("Rotation");
		ImGui::DragFloat3("##Rotation", reinterpret_cast<float*>(&my3DSprite->myRotation), 0.1f);


		ImGui::End();

		auto* spriteDrawer = Engine::GetInstance()->GetSpriteDrawer();

		spriteDrawer->Draw3D(my3DSprite);
	}
	void Graphics::RenderSpritesAndDisplayImages()
	{

		//myDeviceContext->OMSetDepthStencilState(nullptr, 0);

		//auto* spriteDrawer = Engine::GetInstance()->GetSpriteDrawer();
		//auto sprites = Engine::GetInstance()->GetSpriteManager().GetSprites();

		//spriteDrawer->DrawBatch(sprites);
	}

	void Graphics::RestoreRenderTargetsAndStates(ID3D11RenderTargetView* anOldRenderTarget, ID3D11DepthStencilView* anOldDepthStencilView)
	{
		// Restore original render targets and depth stencil view
		myDeviceContext->OMSetRenderTargets(1, &anOldRenderTarget, anOldDepthStencilView);
		myDeviceContext->VSSetSamplers(0, 1, mySamplerState.GetAddressOf());
		myDeviceContext->VSSetSamplers(2, 1, myLinearPointSampler.GetAddressOf());
		myDeviceContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf());
		myDeviceContext->PSSetSamplers(1, 1, myShadowSampler.GetAddressOf());
		myDeviceContext->PSSetSamplers(2, 1, myLinearPointSampler.GetAddressOf());
		myDeviceContext->PSSetSamplers(3, 1, myWaterSampler.GetAddressOf());

		myDeviceContext->CSSetSamplers(2, 1, myLinearPointSampler.GetAddressOf());
		myDeviceContext->CSSetSamplers(0, 1, mySamplerState.GetAddressOf());
		myDeviceContext->CSSetSamplers(1, 1, myShadowSampler.GetAddressOf());

		if (anOldRenderTarget != nullptr)
		{
			anOldRenderTarget->Release();
		}
		if (anOldDepthStencilView != nullptr)
		{
			anOldDepthStencilView->Release();
		}
		//myRasterizerState->Release();
	}

	void Graphics::SortTransparentObjects(std::vector<mi::GameObject*>& transparentObjects)
	{
		transparentObjects;
		//Camera* camera = myCurrentCamera;
		//DirectX::XMVECTOR cameraPosition = camera->GetPositionXMVector();

		//std::sort(transparentObjects.begin(), transparentObjects.end(),
		//	[&cameraPosition](mi::GameObject* a, mi::GameObject* b) -> bool {
		//		DirectX::XMVECTOR posA = a->LocalPosition().AsXMVECTOR();
		//		DirectX::XMVECTOR posB = b->LocalPosition().AsXMVECTOR();
		//		float distA = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(posA, cameraPosition)));
		//		float distB = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(posB, cameraPosition)));
		//		return distA > distB; // Descending order
		//	}
		//);
	}

	void Graphics::RenderTransparentObjects(std::vector<mi::GameObject*>& aGameObjects)
	{
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(0, 1, myEnvironmentTexture.GetAddressOf());

		myDeviceContext->OMSetRenderTargets(1, myRTLighting.GetRenderTargetView().GetAddressOf(), myDepthStencilTarget.GetDepthResourceView().Get());
		//myDeviceContext->ClearDepthStencilView(myDepthStencilTarget.GetDepthResourceView().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		myDeviceContext->OMSetDepthStencilState(myTransDepthStencilState.Get(), 1);
		myDeviceContext->OMSetBlendState(myBlendState.Get(), blendFactor, 0xffffffff);
		for (auto g : aGameObjects)
		{
			g->Render();
		}
		myDeviceContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
	}

	void Graphics::Resize(int aWidth, int aHeight)
	{
		myWidth = aWidth;
		myHeight = aHeight;

		myBackBufferRenderTargetView.Reset();
		myDepthStencilTarget.GetDepthResourceView().Reset();

		HRESULT hr = mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to resize the swap chain.");
			return;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "GetBuffer Failed.");
			return;
		}

		hr = myDevice->CreateRenderTargetView(backBuffer.Get(), NULL, myBackBufferRenderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create render target view.");
			return;
		}

		D3D11_TEXTURE2D_DESC depthTextureDesc;
		depthTextureDesc.Width = myWidth;
		depthTextureDesc.Height = myHeight;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;

		hr = myDevice->CreateTexture2D(&depthTextureDesc, NULL, myDepthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create depth stencil buffer.");
			return;
		}

		hr = myDevice->CreateDepthStencilView(myDepthStencilBuffer.Get(), NULL, myDepthStencilTarget.GetDepthResourceView().GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create depth stencil view.");
			return;
		}

		myDeviceContext->OMSetRenderTargets(1, myBackBufferRenderTargetView.GetAddressOf(), myDepthStencilTarget.GetDepthResourceView().Get());
		myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);

		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(myWidth);
		viewport.Height = static_cast<float>(myHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		myDeviceContext->RSSetViewports(1, &viewport);
	}

	const RenderTarget& Graphics::GetSceneRenderTarget() const
	{
		return myBloomSceneRT;
	}

	bool Graphics::InitDirectX(HWND hwnd)
	{
		RECT rect;
		GetClientRect(myHandle, &rect);
		myWidth = rect.right - rect.left;
		myHeight = rect.bottom - rect.top;

		if (!CreateSwapChain(hwnd))
		{
			return false;
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		HRESULT hr = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "GetBuffer failed stupid.");
			return false;
		}
		hr = myDevice->CreateRenderTargetView(backBuffer.Get(), NULL, myBackBufferRenderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating render target view.");
			return false;
		}
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		//depthStencilDesc.DepthEnable = true;
		//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		//depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;//D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Set to D3D11_DEPTH_WRITE_MASK_ZERO if you don't want to write to the depth buffer
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test parameters
		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = 0xFF;
		dsDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


		hr = myDevice->CreateDepthStencilState(&dsDesc, myDepthStencilState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating depth state.");
			return false;
		}

		dsDesc = {};
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // Disable depth writing
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = false;


		hr = myDevice->CreateDepthStencilState(&dsDesc, &mySkyboxDepthStencilState);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating depth state.");

		}



		myDepthStencilTarget.InitDepthTexture(myWidth, myHeight);

		myDeviceContext->OMSetRenderTargets(1, myBackBufferRenderTargetView.GetAddressOf(), myDepthStencilTarget.GetDepthResourceView().Get());
		//myDeviceContext->OMSetDepthStencilState(myDepthStencilState.Get(), 0);


		// Create a depth stencil state for transparent objects
		D3D11_DEPTH_STENCIL_DESC transDepthStencilDesc;
		ZeroMemory(&transDepthStencilDesc, sizeof(transDepthStencilDesc));
		transDepthStencilDesc.DepthEnable = true;
		transDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable depth writing
		transDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		transDepthStencilDesc.StencilEnable = false; // Assuming stencil test is not needed for transparency


		hr = myDevice->CreateDepthStencilState(&transDepthStencilDesc, myTransDepthStencilState.GetAddressOf());

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
#ifdef ADDITIVE_BLENDING
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
#else
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
#endif
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		hr = myDevice->CreateBlendState(&blendDesc, myBlendState.GetAddressOf());

		// Check for errors and then set the blend state


		ZeroMemory(&mySceneViewport, sizeof(D3D11_VIEWPORT));

		mySceneViewport.TopLeftX = 0;
		mySceneViewport.TopLeftY = 0;
		mySceneViewport.Width = (float)myWidth;
		mySceneViewport.Height = (float)myHeight;
		mySceneViewport.MinDepth = 0.0f;
		mySceneViewport.MaxDepth = 1.0f;
		myDeviceContext->RSSetViewports(1, &mySceneViewport);

		ZeroMemory(&myDepthViewport, sizeof(D3D11_VIEWPORT));

		myDepthViewport.TopLeftX = 0;
		myDepthViewport.TopLeftY = 0;
		myDepthViewport.Width = (float)1024 * DEPTH_MAP_VIEWPORT_SCALE;
		myDepthViewport.Height = (float)1024 * DEPTH_MAP_VIEWPORT_SCALE;
		myDepthViewport.MinDepth = 0.0f;
		myDepthViewport.MaxDepth = 1.0f;


		//Rasterizer state 
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		//rasterizerDesc.AntialiasedLineEnable = true;
		//rasterizerDesc.MultisampleEnable = true;




		hr = myDevice->CreateRasterizerState(&rasterizerDesc, myRasterizerState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed to create rasterizer state.");
			return false;
		}

		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		//rasterizerDesc.AntialiasedLineEnable = true;
		//rasterizerDesc.MultisampleEnable = true;




		hr = myDevice->CreateRasterizerState(&rasterizerDesc, myShadowRasterizerState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed to create rasterizer state.");
			return false;
		}


		rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;  // Disable culling
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = false;


		hr = myDevice->CreateRasterizerState(&rasterizerDesc, &mySkyboxRasterizer);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed to create skybox rasterizer state.");

		}



		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		//samplerDesc.MipLODBias = 6;
		hr = myDevice->CreateSamplerState(&samplerDesc, mySamplerState.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating sampler state.");
			return false;
		}


		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = myDevice->CreateSamplerState(&samplerDesc, myLinearPointSampler.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating sampler state.");
			return false;
		}

		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = 6;
		hr = myDevice->CreateSamplerState(&samplerDesc, myWaterSampler.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating sampler state.");
			return false;
		}


		D3D11_SAMPLER_DESC shadowSamplerDesc;
		ZeroMemory(&shadowSamplerDesc, sizeof(shadowSamplerDesc));
		shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
		shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		shadowSamplerDesc.MaxAnisotropy = 8; // Adjust as necessary, higher for better quality duh
		shadowSamplerDesc.MinLOD = 0;
		shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		shadowSamplerDesc.BorderColor[0] = 1.0f;
		shadowSamplerDesc.BorderColor[1] = 1.0f;
		shadowSamplerDesc.BorderColor[2] = 1.0f;
		shadowSamplerDesc.BorderColor[3] = 1.0f;

		hr = myDevice->CreateSamplerState(&shadowSamplerDesc, myShadowSampler.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create shadow map sampler state.");
			return false;
		}



		return true;
	}

	bool Graphics::InitScene()
	{
		SetCubemapTexture(L"Assets/Textures/Cubemaps/beautysky.dds");
		myMainCamera = new Camera();
		myMainCamera->SetPosition({ 0.0f, 50.0f, -128.0f });
		myMainCamera->SetProjectionValues(70.f, static_cast<float>(myWidth) / static_cast<float>(myHeight), 0.1f, 2000.f);
		myReflectionCamera = new Camera();
		myReflectionCamera->SetPosition({ 0.0f, 50.0f, -128.0f });
		myReflectionCamera->SetProjectionValues(70.f, static_cast<float>(myWidth) / static_cast<float>(myHeight), 0.1f, 2000.f);
		myCurrentCamera = myMainCamera;
		int terrainWidth = 256;
		int terrainHeight = 256;
		float terrainPerlinScale = 50.0f;
		int terrainOctaves = 6;
		float terrainPersistence = 0.4f;
		float terrainLacunarity = 2.0f;
		int terrainSeed = 12345;
		myTerrainGenerator = new TerrainGenerator();
		myTerrainGenerator->Init(myDevice, myDeviceContext, *myCurrentCamera,
			terrainWidth, terrainHeight, terrainPerlinScale, terrainOctaves, terrainPersistence, terrainLacunarity, terrainSeed);
		myTerrainGenerator->myPosition.x -= terrainWidth / 2;
		myTerrainGenerator->myPosition.y -= 40.f;
		myTerrainGenerator->myPosition.z -= terrainHeight / 2;


		Light::DirectionalLight::myDirectionalLightDirection = mi::Vector4f(-0.005f, 0.720f, 0.005f, 1.f);

		//------------------------------------------------------------------------------------------------------
		myRTDepthCamera.Init(myWidth, myHeight);
		myTerrainSceneRT.Init(myWidth, myHeight);
		HRESULT hr = myCBShadowLightBuffer.Init(myDevice.Get(), myDeviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "faiiled creating shadowlight constant buffer");

		}
		hr = myCBLight.Init(myDevice.Get(), myDeviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating light constant buffer.");
			return false;
		}

		hr = myCBCommon.Init(myDevice.Get(), myDeviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating common constant buffer.");
			return false;
		}

		myBloomSceneRT.Init(myWidth, myHeight);
		myBloomRTCombined.Init(myWidth, myHeight);
		myBrightAreasRT.Init(myWidth, myHeight);
		mySceneRT.Init(myWidth, myHeight);

		myShadowMapRenderTarget.Init(1024 * DEPTH_MAP_VIEWPORT_SCALE, 1024 * DEPTH_MAP_VIEWPORT_SCALE);


		myRenderTargetPixelShader.Init(myDevice, L"Shaders/RenderTargetShaderPixel.cso");
		myDepthTargetPixelShader.Init(myDevice, L"Shaders/DepthTargetShaderPixel.cso");
		myBrightAreasExtractionPixelShader.Init(myDevice, L"Shaders/BrightAreasExtractionShaderPixel.cso");
		myBlurPixelShader.Init(myDevice, L"Shaders/BlurShaderPixel.cso");
		myCombineBloomScenePixelShader.Init(myDevice, L"Shaders/CombineBloomAndSceneShaderPixel.cso");

		myLightingPixelShader.Init(myDevice, L"Shaders/LightingPassShaderPixel.cso");


		D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		mi::RenderTargetVertex vertices[] = {
		{-1.0f,  1.0f},   // Top left
		{ 1.0f, -1.0f},  // Bottom right
		{-1.0f, -1.0f},  // Bottom left

		{-1.0f,  1.0f},  // Top left
		{ 1.0f,  1.0f},  // Top right
		{ 1.0f, -1.0f},  // Bottom right
		};
		myRenderTargetVertices.Init(myDevice.Get(), vertices, 6);
		myRenderTargetVertexShader.Init(myDevice, L"Shaders/RenderTargetShaderVertex.cso", layoutDesc, 1);

		myBlurBuffer.Init(myDevice.Get(), myDeviceContext.Get());
		return true;
	}

	bool Graphics::CreateSwapChain(HWND hwnd)
	{

		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
		if (adapters.size() < 1)
		{
			ErrorLog::Log("lmao no IDXGI adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd;
		ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

		scd.BufferDesc.Width = myWidth;
		scd.BufferDesc.Height = myHeight;
		scd.BufferDesc.RefreshRate.Numerator = 75; // TODO: Fix so that it gets the refresh rate from the monitor instead.
		scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1; // Multisamples per pixel
		scd.SampleDesc.Quality = 0;

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = true;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allows toggling between windowed and fullscreen.

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].myAdapter, // GPU, IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, // SOFWARE DRIVER TYPE
			NULL, // FLAGS FOR RUNTIME LAYERS
			NULL, // FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION,
			&scd, // SWAPCHAIN DESCRIPTION
			mySwapChain.GetAddressOf(),
			myDevice.GetAddressOf(),
			NULL, // SUPPORTED FEATURE LEVEL
			myDeviceContext.GetAddressOf());

		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating device and swapchain, damn.");
			return false;
		}
		return true;
	}

	bool Graphics::InitGrid()
	{
		const float numGridCellsX = 200;
		const float numGridCellsY = 200;
		const float gridSize = 10.f;
		const float gridWidth = gridSize * numGridCellsX; // Total width of the grid
		const float gridHeight = gridSize * numGridCellsY; // Total height of the grid

		std::vector<mi::Vertex> gridVertices;

		// Horizontal grid lines
		for (int y = 0; y <= numGridCellsY; ++y)
		{
			float zPos = y * gridSize - gridHeight * 0.5f;

			gridVertices.push_back(mi::Vertex(-gridWidth * 0.5f, 0.0f, zPos, 0.0f, zPos / gridHeight, 0, 0, 0));

			gridVertices.push_back(mi::Vertex(gridWidth * 0.5f, 0.0f, zPos, 1.0f, zPos / gridHeight, 0, 0, 0));
		}

		// Vertical grid lines
		for (int x = 0; x <= numGridCellsX; ++x)
		{
			float xPos = x * gridSize - gridWidth * 0.5f;

			gridVertices.push_back(mi::Vertex(xPos, 0.0f, -gridHeight * 0.5f, xPos / gridWidth, 0.0f, 0, 0, 0));

			gridVertices.push_back(mi::Vertex(xPos, 0.0f, gridHeight * 0.5f, xPos / gridWidth, 1.0f, 0, 0, 0));
		}

		HRESULT hr = myGridVertexBuffer.Init(myDevice.Get(), &gridVertices[0], static_cast<UINT>(gridVertices.size()));

		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed initializing grid vertex buffer.");
			return false;

		}

		std::vector<DWORD> gridIndices;

		for (UINT i = 0; i < gridVertices.size(); i += 2)
		{
			gridIndices.push_back(i);
			gridIndices.push_back(i + 1);
		}

		hr = myGridIndexBuffer.Init(myDevice.Get(), &gridIndices[0], static_cast<UINT>(gridIndices.size()));
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed initializing grid indices buffer.");
			return false;
		}

		hr = myGridConstantBuffer.Init(myDevice.Get(), myDeviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed initializing grid constant buffer.");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		UINT numElements = ARRAYSIZE(layout);

		if (!myLineVertexShader.Init(myDevice, L"shaders/LineShaderVertex.cso", layout, numElements))
		{
			return false;
		}

		if (!myLinePixelShader.Init(myDevice, L"shaders/LineShaderPixel.cso"))
		{
			return false;
		}

		return true;
	}

	void Graphics::RenderGrid()
	{
		UINT stride = myGridVertexBuffer.GetStride();
		UINT offset = 0;


		myGridConstantBuffer.myData.ObjectWorldMatrix = myCurrentCamera->GetViewMatrix() * myCurrentCamera->GetProjectionMatrix();
		myGridConstantBuffer.myData.ObjectWorldMatrix = XMMatrixTranspose(myGridConstantBuffer.myData.ObjectWorldMatrix);
		myDeviceContext->UpdateSubresource(myGridConstantBuffer.Get(), 0, nullptr, &myGridConstantBuffer.myData, 0, 0);
		myGridConstantBuffer.ApplyChanges();
		//TODO: FIX, CANT SET CONSTANT BUFFER TO 0 HERE ALREADY TAKE UP BY COMMON BUFFER
		myDeviceContext->VSSetConstantBuffers(0, 1, myGridConstantBuffer.GetAddressOf());

		myDeviceContext->IASetInputLayout(myLineVertexShader.GetInputLayout());
		myDeviceContext->VSSetShader(myLineVertexShader.GetShader(), NULL, 0);
		myDeviceContext->PSSetShader(myLinePixelShader.GetShader(), NULL, 0);

		myDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		myDeviceContext->IASetVertexBuffers(0, 1, myGridVertexBuffer.GetAddressOf(), &stride, &offset);
		myDeviceContext->IASetIndexBuffer(myGridIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		myDeviceContext->DrawIndexed(myGridIndexBuffer.GetBufferSize(), 0, 0);
	}
	// TODO:: CHANGE TO MEMCPY

	bool Graphics::InitSkybox()
	{
		const float skyboxSize = 1000.0f;

		std::vector<mi::Vertex> skyboxVertices = {
			// Front face
			mi::Vertex(-skyboxSize, -skyboxSize, -skyboxSize, 0.0f, 1.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, skyboxSize, -skyboxSize, 0.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, skyboxSize, -skyboxSize, 1.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, -skyboxSize, -skyboxSize, 1.0f, 1.0f, 0, 0, 0),

			// Back face
			mi::Vertex(skyboxSize, -skyboxSize, skyboxSize, 0.0f, 1.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, skyboxSize, skyboxSize, 0.0f, 0.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, skyboxSize, skyboxSize, 1.0f, 0.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, -skyboxSize, skyboxSize, 1.0f, 1.0f, 0, 0, 0),

			// Left face
			mi::Vertex(-skyboxSize, -skyboxSize, skyboxSize, 0.0f, 1.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, skyboxSize, skyboxSize, 0.0f, 0.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, skyboxSize, -skyboxSize, 1.0f, 0.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, -skyboxSize, -skyboxSize, 1.0f, 1.0f, 0, 0, 0),

			// Right face
			mi::Vertex(skyboxSize, -skyboxSize, -skyboxSize, 0.0f, 1.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, skyboxSize, -skyboxSize, 0.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, skyboxSize, skyboxSize, 1.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, -skyboxSize, skyboxSize, 1.0f, 1.0f, 0, 0, 0),

			// Top face
			mi::Vertex(-skyboxSize, skyboxSize, -skyboxSize, 0.0f, 1.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, skyboxSize, skyboxSize, 0.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, skyboxSize, skyboxSize, 1.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, skyboxSize, -skyboxSize, 1.0f, 1.0f, 0, 0, 0),

			// Bottom face
			mi::Vertex(-skyboxSize, -skyboxSize, skyboxSize, 0.0f, 1.0f, 0, 0, 0),
			mi::Vertex(-skyboxSize, -skyboxSize, -skyboxSize, 0.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, -skyboxSize, -skyboxSize, 1.0f, 0.0f, 0, 0, 0),
			mi::Vertex(skyboxSize, -skyboxSize, skyboxSize, 1.0f, 1.0f, 0, 0, 0)
		};

		HRESULT hr = mySkyboxVertexBuffer.Init(myDevice.Get(), &skyboxVertices[0], static_cast<UINT>(skyboxVertices.size()));
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed initializing skybox vertex buffer.");
			return false;
		}

		std::vector<DWORD> skyboxIndices = {
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		hr = mySkyboxIndexBuffer.Init(myDevice.Get(), &skyboxIndices[0], static_cast<UINT>(skyboxIndices.size()));
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed initializing skybox index buffer.");
			return false;
		}

		hr = mySkyboxConstantBuffer.Init(myDevice.Get(), myDeviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed initializing skybox constant buffer.");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		UINT numElements = ARRAYSIZE(layout);

		if (!mySkyboxVertexShader.Init(myDevice, L"shaders/SkyboxShaderVertex.cso", layout, numElements))
		{
			return false;
		}

		if (!mySkyboxPixelShader.Init(myDevice, L"shaders/SkyboxShaderPixel.cso"))
		{
			return false;
		}

		return true;

	}

	void Graphics::RenderSkybox()
	{
		UINT stride = mySkyboxVertexBuffer.GetStride();
		UINT offset = 0;

		myDeviceContext->PSSetShaderResources(0, 1, myEnvironmentTexture.GetAddressOf());

		myDeviceContext->IASetInputLayout(mySkyboxVertexShader.GetInputLayout());
		myDeviceContext->VSSetShader(mySkyboxVertexShader.GetShader(), NULL, 0);
		myDeviceContext->PSSetShader(mySkyboxPixelShader.GetShader(), NULL, 0);

		myDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myDeviceContext->IASetVertexBuffers(0, 1, mySkyboxVertexBuffer.GetAddressOf(), &stride, &offset);
		myDeviceContext->IASetIndexBuffer(mySkyboxIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		myDeviceContext->DrawIndexed(mySkyboxIndexBuffer.GetBufferSize(), 0, 0);
	}


	void Graphics::UpdateLightBuffer()
	{
		Light::DirectionalLight dirLight;
		myCBLight.myData.LightColor = XMFLOAT3(dirLight.myDirectionalLightColor.x, dirLight.myDirectionalLightColor.y, dirLight.myDirectionalLightColor.z);
		myCBLight.myData.Intensity = dirLight.myDirectionalLightIntensity;
		myCBLight.myData.AmbientLightColor = XMFLOAT4(dirLight.myAmbientColorAndIntensity.x, dirLight.myAmbientColorAndIntensity.y, dirLight.myAmbientColorAndIntensity.z, dirLight.myAmbientColorAndIntensity.w);

		XMFLOAT4 dir = XMFLOAT4(dirLight.myDirectionalLightDirection.x, dirLight.myDirectionalLightDirection.y, dirLight.myDirectionalLightDirection.z, dirLight.myDirectionalLightDirection.w);


		myCBLight.myData.Direction = dir;
		// TODO::CHANGE TO MEMCPY

		myCBLight.myData.NumberOfPointLights = (int)Light::RenderedPointLights.size();
		for (size_t i = 0; i < myCBLight.myData.NumberOfPointLights; i++)
		{

			myCBLight.myData.myPointLights[i].Position.x = Light::RenderedPointLights[i]->myPosition.x;
			myCBLight.myData.myPointLights[i].Position.y = Light::RenderedPointLights[i]->myPosition.y;
			myCBLight.myData.myPointLights[i].Position.z = Light::RenderedPointLights[i]->myPosition.z;
			myCBLight.myData.myPointLights[i].Position.w = Light::RenderedPointLights[i]->myPosition.w;

			myCBLight.myData.myPointLights[i].Color.x = Light::RenderedPointLights[i]->myColor.x;
			myCBLight.myData.myPointLights[i].Color.y = Light::RenderedPointLights[i]->myColor.y;
			myCBLight.myData.myPointLights[i].Color.z = Light::RenderedPointLights[i]->myColor.z;
			myCBLight.myData.myPointLights[i].Color.w = Light::RenderedPointLights[i]->myColor.w;



			myCBLight.myData.myPointLights[i].Range = Light::RenderedPointLights[i]->myRange;
		}

		// TODO:: CHANGE TO MEMCPY
		myCBLight.myData.NumberOfSpotLights = (int)Light::RenderedSpotLights.size();
		for (size_t i = 0; i < myCBLight.myData.NumberOfSpotLights; i++)
		{
			myCBLight.myData.mySpotLights[i].Position.x = Light::RenderedSpotLights[i]->myPosition.x;
			myCBLight.myData.mySpotLights[i].Position.y = Light::RenderedSpotLights[i]->myPosition.y;
			myCBLight.myData.mySpotLights[i].Position.z = Light::RenderedSpotLights[i]->myPosition.z;
			myCBLight.myData.mySpotLights[i].Position.w = Light::RenderedSpotLights[i]->myPosition.w;

			myCBLight.myData.mySpotLights[i].Color.x = Light::RenderedSpotLights[i]->myColor.x;
			myCBLight.myData.mySpotLights[i].Color.y = Light::RenderedSpotLights[i]->myColor.y;
			myCBLight.myData.mySpotLights[i].Color.z = Light::RenderedSpotLights[i]->myColor.z;
			myCBLight.myData.mySpotLights[i].Color.w = Light::RenderedSpotLights[i]->myColor.w;
			// TODO: try using static cast or reinterpret cast. fuck even dynamic cast.
			myCBLight.myData.mySpotLights[i].Range = Light::RenderedSpotLights[i]->myRange;
			myCBLight.myData.mySpotLights[i].Direction.x = Light::RenderedSpotLights[i]->myDirection.x;
			myCBLight.myData.mySpotLights[i].Direction.y = Light::RenderedSpotLights[i]->myDirection.y;
			myCBLight.myData.mySpotLights[i].Direction.z = Light::RenderedSpotLights[i]->myDirection.z;

			myCBLight.myData.mySpotLights[i].OuterAngle = Light::RenderedSpotLights[i]->myOuterAngle;
			myCBLight.myData.mySpotLights[i].InnerAngle = Light::RenderedSpotLights[i]->myInnerAngle;

			myCBLight.myData.mySpotLights[i].Range = Light::RenderedSpotLights[i]->myRange;
		}


		myCBLight.ApplyChanges();
		myDeviceContext->PSSetConstantBuffers(1, 1, myCBLight.GetAddressOf());
		myDeviceContext->VSSetConstantBuffers(1, 1, myCBLight.GetAddressOf());

	}

	void Graphics::RenderLightPropertyWindows()
	{

		ImGui::Begin("Directional Light Properties");
		ImGui::ColorEdit4("Light Color", reinterpret_cast<float*>(&Light::DirectionalLight::myDirectionalLightColor));
		ImGui::DragFloat("Intensity", reinterpret_cast<float*>(&Light::DirectionalLight::myDirectionalLightIntensity), 0.05f);
		ImGui::DragFloat4("Direction", reinterpret_cast<float*>(&Light::DirectionalLight::myDirectionalLightDirection), 0.005F, -1.f, 1.f);

		ImGui::End();
	}

	void Graphics::UpdateConstantBuffers()
	{

		myCBShadowLightBuffer.myData.DirLightViewMatrix = myDirectionalLightCamera.GetViewMatrix();
		myCBShadowLightBuffer.myData.DirProjectionMatrix = myDirectionalLightCamera.GetProjectionMatrix();
		myCBShadowLightBuffer.ApplyChanges();

		Camera& camera = *myCurrentCamera;
		XMFLOAT4 camPos;
		camPos.x = camera.GetPosition().x;
		camPos.y = camera.GetPosition().y;
		camPos.z = camera.GetPosition().z;
		camPos.w = 1;
		myCBCommon.myData.CameraPosition = camPos;
		myCBCommon.myData.ProjectionMatrix = camera.GetProjectionMatrix();
		myCBCommon.myData.ViewMatrix = camera.GetViewMatrix();

		myCBCommon.ApplyChanges();


		myDeviceContext->VSSetConstantBuffers(4, 1, myCBShadowLightBuffer.GetAddressOf());
		myDeviceContext->PSSetConstantBuffers(4, 1, myCBShadowLightBuffer.GetAddressOf());

		myDeviceContext->VSSetConstantBuffers(0, 1, myCBCommon.GetAddressOf());
		myDeviceContext->PSSetConstantBuffers(0, 1, myCBCommon.GetAddressOf());
		myDeviceContext->GSSetConstantBuffers(0, 1, myCBCommon.GetAddressOf());

	}

	void Graphics::ExtractBrightAreas()
	{

		myBrightAreasRT.SetAsRenderTarget(myDeviceContext.Get(), nullptr);


		myDeviceContext->PSSetShader(myBrightAreasExtractionPixelShader.GetShader(), nullptr, 0);
		myDeviceContext->PSSetShaderResources(0, 1, mySceneRT.GetShaderResourceView().GetAddressOf());
		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullSRV[] = { nullptr };
		myDeviceContext->PSSetShaderResources(0, 1, nullSRV);
	}

	void Graphics::BlurBloomTexture()
	{
		// Horizontal pass
		Blur(myBloomSceneRT.GetRenderTargetView().Get(), myBrightAreasRT.GetShaderResourceView().Get(), true);
		// Vertical pass, blurs the vertical of the already blurred myBloomSceneRT, which currently is just horizontal blur, check opengl bloom guide if you dont understand dummy.
		Blur(myBloomRTCombined.GetRenderTargetView().Get(), myBloomSceneRT.GetShaderResourceView().Get(), false);

	}

	void Graphics::Blur(ID3D11RenderTargetView* outputRTV, ID3D11ShaderResourceView* inputSRV, bool isHorizontal)
	{

		myDeviceContext->OMSetRenderTargets(1, &outputRTV, nullptr);


		myDeviceContext->PSSetShader(myBlurPixelShader.GetShader(), nullptr, 0);
		myDeviceContext->PSSetShaderResources(0, 1, &inputSRV);

		myBlurBuffer.myData.IsHorizontal = isHorizontal;
		myBlurBuffer.myData.RenderResolution = XMFLOAT2((float)myWidth, (float)myHeight);
		myBlurBuffer.ApplyChanges();
		myDeviceContext->PSSetConstantBuffers(0, 1, myBlurBuffer.GetAddressOf());

		RenderFullScreenQuad();


		ID3D11ShaderResourceView* nullSRV[] = { nullptr };
		myDeviceContext->PSSetShaderResources(0, 1, nullSRV);
	}

	void Graphics::CombineBloomWithScene()
	{

		myDeviceContext->PSSetShader(myCombineBloomScenePixelShader.GetShader(), nullptr, 0);
		myDeviceContext->OMSetRenderTargets(1, myBloomSceneRT.GetRenderTargetView().GetAddressOf(), nullptr);

		myDeviceContext->PSSetShaderResources(0, 1, mySceneRT.GetShaderResourceView().GetAddressOf());
		myDeviceContext->PSSetShaderResources(1, 1, myBloomRTCombined.GetShaderResourceView().GetAddressOf());

		RenderFullScreenQuad();

		ID3D11ShaderResourceView* nullSRV[] = { nullptr, nullptr };
		myDeviceContext->PSSetShaderResources(0, 2, nullSRV);
		myDeviceContext->PSSetShaderResources(1, 2, nullSRV);
	}

	void Graphics::RenderFullScreenQuad()
	{
		myDeviceContext->VSSetShader(myRenderTargetVertexShader.GetShader(), nullptr, 0);

		myDeviceContext->IASetInputLayout(myRenderTargetVertexShader.GetInputLayout());

		myDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		UINT offset = 0;
		myDeviceContext->IASetVertexBuffers(0, 1, myRenderTargetVertices.GetAddressOf(), myRenderTargetVertices.GetStridePtr(), &offset);


		myDeviceContext->Draw(4, 0);
	}
}
