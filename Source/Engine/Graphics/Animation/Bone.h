#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "BoneInfo.h"

struct aiNodeAnim;

namespace mi
{
	struct KeyPosition
	{
		DirectX::XMFLOAT3 position;
		float timeStamp;
	};

	struct KeyRotation
	{
		DirectX::XMFLOAT4 orientation;
		float timeStamp;
	};

	struct KeyScale
	{
		DirectX::XMFLOAT3 scale;
		float timeStamp;
	};

	class Bone
	{
	private:
		std::vector<KeyPosition> myPositions;
		std::vector<KeyRotation> myRotations;
		std::vector<KeyScale> myScales;
		int myNumPositions;
		int myNumRotations;
		int myNumScalings;

		DirectX::XMMATRIX myLocalTransform;
		std::string myName;
		int myID;

	public:

		Bone() = default;
		Bone(const std::string& name, int ID, const aiNodeAnim* channel);

		void Update(float animationTime, DirectX::XMMATRIX aPreRotation);


		inline DirectX::XMMATRIX GetLocalTransform() { return myLocalTransform; }
		inline std::string GetBoneName() const { return myName; }
		inline int GetBoneID() { return myID; }

		inline void SetRotationVector(std::vector<KeyRotation> aVector)
		{
			myRotations = aVector;
			myNumRotations = (int)aVector.size();
		}
		inline void SetPositionVector(std::vector<KeyPosition> aVector)
		{
			myPositions = aVector;
			myNumPositions = (int)aVector.size();
		}
		inline void SetScaleVector(std::vector<KeyScale> aVector)
		{
			myScales = aVector;
			myNumScalings = (int)aVector.size();
		}
		inline std::vector<KeyRotation> GetRotations() { return myRotations; }
		inline std::vector<KeyPosition> GetPositions() { return myPositions; }
		inline std::vector<KeyScale> GetScales() { return myScales; }

		int GetPositionIndex(float animationTime);

		int GetRotationIndex(float animationTime);

		int GetScaleIndex(float animationTime);


	private:

		/* Gets normalized value for Lerp & Slerp*/
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		DirectX::XMMATRIX InterpolatePosition(float animationTime);

		DirectX::XMMATRIX InterpolateRotation(float animationTime);

		DirectX::XMMATRIX InterpolateScaling(float animationTime);


	};
}