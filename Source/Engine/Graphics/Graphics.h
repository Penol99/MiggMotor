#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include <WICTextureLoader.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <filesystem>
#include <Timer.h>
#include "../Graphics/Texture.h"
#include "RenderTarget.h"

using namespace Microsoft::WRL;

namespace mi
{
	class PixelShader;
	class SpriteDrawer;
	class Sprite3D;
	class Animation;
	class GameObject;
	class FFTWaterSimulation;
	class TerrainGenerator;
	class Graphics
	{
	public:

		~Graphics();
		bool Init(HWND hwnd, int aWidth, int aHeight, Timer& aTimer);
		void SetCubemapTexture(const std::wstring& cubemapDDSFilePath);
		void Render2D();

		void Render3D(std::vector<mi::GameObject*>& aGameObjects);
		void Resize(int aWidth, int aHeight);

		const RenderTarget& GetSceneRenderTarget() const;
		Camera *myCurrentCamera;
		Camera* myMainCamera;
		Camera *myReflectionCamera;


	private:
		bool InitDirectX(HWND hwnd);
		bool InitScene();
		bool CreateSwapChain(HWND hwnd);
		bool InitGrid();
		void InitializeDeferredBuffers();

		// Render Pipeline
		void RenderShadowPass(std::vector<mi::GameObject*>& someGameObjects);
		void RenderSceneToGBuffers(std::vector<mi::GameObject*>& aGameObjects);
		void PerformLightingPass();
		void RenderGrid();
		void Render3DSprites();
		void RenderSpritesAndDisplayImages();
		void RestoreRenderTargetsAndStates(ID3D11RenderTargetView* anOldRenderTarget, ID3D11DepthStencilView* anOldDepthStencilView);
		void SortTransparentObjects(std::vector<mi::GameObject*>& transparentObjects);
		void RenderTransparentObjects(std::vector<mi::GameObject*>& aGameObjects);



		// Bloom
		void ExtractBrightAreas();
		void BlurBloomTexture();
		void Blur(ID3D11RenderTargetView* outputRTV, ID3D11ShaderResourceView* inputSRV, bool isHorizontal);
		void CombineBloomWithScene();
		void RenderFullScreenQuad();
		bool InitSkybox();
		void RenderSkybox();
		// Misc
		void UpdateLightBuffer();
		void UpdateConstantBuffers();
		void RenderLightPropertyWindows();

		float myBackgroundColor[4] = { .2f, .2f, .2f, 1.0f };

		std::vector<mi::GameObject*> myTransparentObjects;

		D3D11_VIEWPORT mySceneViewport;
		D3D11_VIEWPORT myDepthViewport;


		ComPtr<ID3D11ShaderResourceView> myEnvironmentTexture;

		Camera myDirectionalLightCamera;

		ConstantBuffer<CB_BlurBuffer> myBlurBuffer;
		ConstantBuffer<CB_ShadowBuffer> myCBShadowLightBuffer;

		RenderTarget myRTPosition;
		RenderTarget myRTNormal;
		RenderTarget myRTAlbedo;
		RenderTarget myRTInputNormals;
		RenderTarget myRTMaterial;
		RenderTarget myRTDepthCamera;

		RenderTarget myRTDirectionalLightPos;

		RenderTarget myRTLighting;




		RenderTarget myDepthStencilTarget;
		RenderTarget myShadowMapRenderTarget;
		RenderTarget myBloomSceneRT;
		RenderTarget myBloomRTCombined;
		RenderTarget myBrightAreasRT;
		RenderTarget mySceneRT;
		RenderTarget myTerrainSceneRT;

		PixelShader myLightingPixelShader;

		PixelShader myBrightAreasExtractionPixelShader;
		PixelShader myBlurPixelShader;
		PixelShader myCombineBloomScenePixelShader;


		VertexShader myRenderTargetVertexShader;
		PixelShader myRenderTargetPixelShader;
		PixelShader myDepthTargetPixelShader;
		VertexBuffer<mi::RenderTargetVertex> myRenderTargetVertices;

		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myDeviceContext;
		ComPtr<IDXGISwapChain> mySwapChain;
		ComPtr<ID3D11RenderTargetView> myBackBufferRenderTargetView;
		ConstantBuffer<CB_ObjectWorldMatrix> myCBVSVertexShader;

		ComPtr<ID3D11Texture2D> myDepthStencilBuffer;
		ComPtr<ID3D11DepthStencilState> myDepthStencilState;
		ComPtr<ID3D11DepthStencilState> mySkyboxDepthStencilState;
		ComPtr<ID3D11DepthStencilState> myTransDepthStencilState;

		ComPtr<ID3D11RasterizerState> myRasterizerState;
		ComPtr<ID3D11RasterizerState> myShadowRasterizerState;
		ComPtr<ID3D11RasterizerState> mySkyboxRasterizer;
		ComPtr<ID3D11BlendState> myBlendState;


		ComPtr<ID3D11SamplerState> mySamplerState;
		ComPtr<ID3D11SamplerState> myLinearPointSampler;
		ComPtr<ID3D11SamplerState> myShadowSampler;
		ComPtr<ID3D11SamplerState> myWaterSampler;
		ComPtr<ID3D11ShaderResourceView> myTexture;

		VertexBuffer<mi::Vertex> myGridVertexBuffer;
		VertexBuffer<mi::Vertex> mySkyboxVertexBuffer;

		IndexBuffer myGridIndexBuffer;
		IndexBuffer mySkyboxIndexBuffer;
		ConstantBuffer<CB_ObjectWorldMatrix> myGridConstantBuffer;
		ConstantBuffer<CB_ObjectWorldMatrix> mySkyboxConstantBuffer;
		ConstantBuffer<CB_CommonBuffer> myCBCommon;

		VertexShader myLineVertexShader;
		VertexShader mySkyboxVertexShader;
		PixelShader myLinePixelShader;
		PixelShader mySkyboxPixelShader;

		GameObject* mySelectedGameObject;

		SpriteDrawer* mySpriteDrawer;
		Timer* myTimer;

		ConstantBuffer<CB_LightConstantBufferData> myCBLight;
		Sprite3D* my3DSprite;

		TerrainGenerator* myTerrainGenerator;

		int myWidth;
		int myHeight;
		int myCurrentRenderPass = 0;
		float myCameraSpeed = 20.0f;
		HWND myHandle;

		friend class ImGuiRenderer;
		friend class SceneSerializer;
		friend class GraphicsPlayground;
		friend class Engine;

	};
}
