#include "Shaders.h"

namespace mi
{
	VertexShader::VertexShader()
		:myShader(nullptr),
		myShaderBuffer(nullptr)
	{
	}
	// Vertex Shader
	bool VertexShader::Init(ComPtr<ID3D11Device>& aDevice, std::wstring aShaderPath, D3D11_INPUT_ELEMENT_DESC* aLayoutDesc, UINT aNumElements)
	{

		HRESULT hr = D3DReadFileToBlob(aShaderPath.c_str(), myShaderBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"failed loading shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;

		}


		if (aLayoutDesc != nullptr)
		{
			hr = aDevice->CreateInputLayout(aLayoutDesc, aNumElements, GetBuffer()->GetBufferPointer(), GetBuffer()->GetBufferSize(), myInputLayout.GetAddressOf());
			if (FAILED(hr))
			{
				ErrorLog::Log(hr, "failed creating input layout, rip.");
				return false;
			}
		}

		hr = aDevice->CreateVertexShader(myShaderBuffer.Get()->GetBufferPointer(), myShaderBuffer.Get()->GetBufferSize(), NULL, myShader.GetAddressOf());

		if (FAILED(hr))
		{
			std::wstring errorMsg = L"failed creating vertex shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;
		}
		return true;
	}

	ID3D11VertexShader* VertexShader::GetShader()
	{
		return myShader.Get();
	}

	ID3D10Blob* VertexShader::GetBuffer()
	{
		return myShaderBuffer.Get();
	}

	ID3D11InputLayout* VertexShader::GetInputLayout()
	{
		return myInputLayout.Get();
	}

	// Pixel Shader
	bool PixelShader::Init(ComPtr<ID3D11Device>& aDevice, std::wstring aShaderPath)
	{
		HRESULT hr = D3DReadFileToBlob(aShaderPath.c_str(), myShaderBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"failed loading shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;

		}

		hr = aDevice->CreatePixelShader(myShaderBuffer.Get()->GetBufferPointer(), myShaderBuffer.Get()->GetBufferSize(), NULL, myShader.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"failed creating pixel shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;
		}
		return true;
	}

	ID3D11PixelShader* PixelShader::GetShader()
	{
		return myShader.Get();
	}

	ID3D10Blob* PixelShader::GetBuffer()
	{
		return myShaderBuffer.Get();
	}

	// Compute Shader

	bool ComputeShader::Init(ComPtr<ID3D11Device>& aDevice, const std::wstring& aShaderPath)
	{
		HRESULT hr = D3DReadFileToBlob(aShaderPath.c_str(), myShaderBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"Failed loading compute shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;
		}

		hr = aDevice->CreateComputeShader(myShaderBuffer.Get()->GetBufferPointer(), myShaderBuffer.Get()->GetBufferSize(), NULL, myShader.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"Failed creating compute shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;
		}

		return true;
	}

	ID3D11ComputeShader* ComputeShader::GetShader() const
	{
		return myShader.Get();
	}

	ID3D10Blob* ComputeShader::GetBuffer()
	{
		return myShaderBuffer.Get();
	}


	GeometryShader::GeometryShader()
		: myShader(nullptr)
		, myShaderBuffer(nullptr)
	{
	}

	bool GeometryShader::Init(ComPtr<ID3D11Device>& aDevice, std::wstring aShaderPath)
	{
		HRESULT hr = D3DReadFileToBlob(aShaderPath.c_str(), myShaderBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"failed loading shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;
		}

		hr = aDevice->CreateGeometryShader(myShaderBuffer.Get()->GetBufferPointer(), myShaderBuffer.Get()->GetBufferSize(), NULL, myShader.GetAddressOf());
		if (FAILED(hr))
		{
			std::wstring errorMsg = L"failed creating geometry shader: ";
			errorMsg += aShaderPath;
			ErrorLog::Log(hr, errorMsg);
			return false;
		}
		return true;
	}

	ID3D11GeometryShader* GeometryShader::GetShader()
	{
		return myShader.Get();
	}

	ID3D10Blob* GeometryShader::GetBuffer()
	{
		return myShaderBuffer.Get();
	}
}
