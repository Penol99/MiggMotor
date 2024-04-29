#pragma once
#include <map>

namespace mi
{
	struct BoneInfo
	{
		~BoneInfo()
		{

		}
		int myID;
		DirectX::XMMATRIX offset;
	};

	
}