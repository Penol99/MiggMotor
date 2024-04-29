#include "engine_pch.h"
#include "AdapterReader.h"

namespace mi
{


	std::vector<AdapterData> AdapterReader::myAdapters;
	std::vector<AdapterData> AdapterReader::GetAdapters()
	{
		if (myAdapters.size() > 0) // already initialized
		{
			return myAdapters;
		}

		Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;


		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating DXGIFactory for enumerating adapters");
			exit(-1);
		}

		IDXGIAdapter* pAdapter;
		UINT index = 0;
		while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
		{
			myAdapters.push_back(AdapterData(pAdapter));
			index += 1;
		}
		return myAdapters;
	}

	AdapterData::AdapterData(IDXGIAdapter* pAdapter)
		:myAdapter(pAdapter)
	{
		HRESULT hr = myAdapter->GetDesc(&this->myDescription);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed getting the description for IDXGIAdapter");
		}
	}
}
