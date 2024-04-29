#include "engine_pch.h"
#include "RenderTarget.h"
#include "Engine.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

namespace mi
{
	void RenderTarget::Init(int aWidth, int aHeight, DXGI_FORMAT format, bool useMips)
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};

		textureDesc.Width = aWidth;
		textureDesc.Height = aHeight;
		textureDesc.MipLevels = useMips ? 0 : 1; // Use 0 for automatic mipmap generation, or 1 for no mipmaps
		textureDesc.ArraySize = 1;

		textureDesc.Format = format;

		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = useMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0; // Enable automatic mipmap generation if useMips is true

		auto& device = Engine::GetInstance()->GetDevice();
		HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &myTexture);

		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating texture2D for render target view");
			return;
		}

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateRenderTargetView(myTexture, &renderTargetViewDesc, myRenderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating render target view");
			return;
		}

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = useMips ? -1 : 1; // Use -1 for all mip levels, or 1 for a single mip level

		hr = device->CreateShaderResourceView(myTexture, &shaderResourceViewDesc, myShaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating shader resource view");
			return;
		}
		myTexture->Release();
	}

	void RenderTarget::InitDepthTexture(int aWidth, int aHeight)
	{
		D3D11_TEXTURE2D_DESC depthTextureDesc = {};
		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};


		depthTextureDesc.Width = aWidth;
		depthTextureDesc.Height = aHeight;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;

		auto& device = Engine::GetInstance()->GetDevice();
		HRESULT hr = device->CreateTexture2D(&depthTextureDesc, NULL, &myTexture);

		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating texture2D for depth stencil");
			return;
		}

		DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DepthStencilViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(myTexture, &DepthStencilViewDesc, myDepthStencilView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating depth stencil view");
			return;
		}

		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		//depthTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateShaderResourceView(myTexture, &shaderResourceViewDesc, myShaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating depth resource view ");
			return;
		}
		myTexture->Release();
	}

	ComPtr<ID3D11RenderTargetView>& RenderTarget::GetRenderTargetView()
	{
		return myRenderTargetView;
	}

	ComPtr<ID3D11ShaderResourceView>& RenderTarget::GetShaderResourceView()
	{
		return myShaderResourceView;
	}

	const ComPtr<ID3D11ShaderResourceView>& RenderTarget::GetShaderResourceView() const
	{
		return myShaderResourceView;
	}

	ComPtr<ID3D11DepthStencilView>& RenderTarget::GetDepthResourceView()
	{
		return myDepthStencilView;
	}

	void RenderTarget::SetAsRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
	{

		deviceContext->OMSetRenderTargets(1, myRenderTargetView.GetAddressOf(), depthStencilView);
	}



	void UAVRenderTarget::Init(int aWidth, int aHeight, DXGI_FORMAT format, bool useMips)
	{

		D3D11_TEXTURE2D_DESC textureDesc = {};
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};

		textureDesc.Width = aWidth;
		textureDesc.Height = aHeight;
		textureDesc.MipLevels = useMips ? 0 : 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;



		auto& device = Engine::GetInstance()->GetDevice();
		HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &myTexture);

		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating texture2D for render target view");
			return;
		}

		// Create UAV
		if (myUAV != nullptr) 
		{
			myUAV.Reset();
			myShaderResourceView.Reset();
		}
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		
		hr = device->CreateUnorderedAccessView(myTexture, &uavDesc, &myUAV);
		if (FAILED(hr)) 
		{
			ErrorLog::Log(hr, "Failed Creating UAV");
			return;
		}

		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = useMips ? -1 : 1;
		hr = device->CreateShaderResourceView(myTexture, &shaderResourceViewDesc, myShaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed creating shader resource view");
			return;
		}
		myTexture->Release();

	}

	ComPtr<ID3D11UnorderedAccessView>& UAVRenderTarget::GetUAV()
	{
		return myUAV;
	}
	
	ComPtr<ID3D11ShaderResourceView>& UAVRenderTarget::GetShaderResourceView()
	{
		return myShaderResourceView;
	}

	const ComPtr<ID3D11ShaderResourceView>& UAVRenderTarget::GetShaderResourceView() const
	{
		return myShaderResourceView;
	}

}
