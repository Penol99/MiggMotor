#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

namespace mi
{
	class RenderTarget
	{
	public:
		void Init(int aWidth, int aHeight, DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT, bool useMips = false);
		void InitDepthTexture(int aWidth, int aHeight);
		ComPtr<ID3D11RenderTargetView>& GetRenderTargetView();
		ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView();
		const ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const;
		ComPtr<ID3D11DepthStencilView>& GetDepthResourceView();
		void SetAsRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);

	private:

		ComPtr<ID3D11RenderTargetView> myRenderTargetView;
		ComPtr<ID3D11ShaderResourceView> myShaderResourceView;

		ComPtr<ID3D11DepthStencilView> myDepthStencilView;
		ID3D11Texture2D* myTexture;
	};

	class UAVRenderTarget
	{
	public:
		void Init(int aWidth, int aHeight, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, bool useMips = false);
		ComPtr<ID3D11UnorderedAccessView>& GetUAV();
		ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView();
		const ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const;

	private:

		ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
		ComPtr<ID3D11UnorderedAccessView> myUAV;
		ID3D11Texture2D* myTexture;
	};

}
