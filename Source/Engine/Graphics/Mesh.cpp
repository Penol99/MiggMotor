#include "engine_pch.h"
#include "Mesh.h"
#include "ErrorLog.h"
#include <d3d11.h> 

namespace mi 
{
	Mesh::Mesh()
	{
		myVertexBuffer;
		myIndexBuffer;
	}

	void Mesh::Render(ID3D11DeviceContext* aDeviceContext)
	{

		UINT offset = 0;

		aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		aDeviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), myVertexBuffer.GetStridePtr(), &offset);
		aDeviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		aDeviceContext->DrawIndexed(myIndexBuffer.GetBufferSize(), 0, 0);
	}

	Mesh::~Mesh()
	{
		int a = 1;
		a;
	}

	bool Mesh::Init(ComPtr<ID3D11Device>& aDevice, std::vector<PBRVertex>& vertices, std::vector<DWORD>& indices, UINT numVertices, UINT numIndices)
	{
		HRESULT hr = myVertexBuffer.Init(aDevice.Get(), &vertices[0], numVertices);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create vertex buffer.");
			return false;

		}

		hr = myIndexBuffer.Init(aDevice.Get(), &indices[0], numIndices);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create index buffer.");
			return false;

		}
		return true;
	}
}
