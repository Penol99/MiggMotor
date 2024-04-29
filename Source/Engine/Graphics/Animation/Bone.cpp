#include "engine_pch.h"
#include "Bone.h"
#include <scene.h>
#include <anim.h>
#include <iostream>
#include <FMath.h>

namespace mi
{

	Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		:
		myName(name),
		myID(ID),
		myLocalTransform()
	{
		myNumPositions = channel->mNumPositionKeys;

		for (int positionIndex = 0; positionIndex < myNumPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			data.position = DirectX::XMFLOAT3(aiPosition.x, aiPosition.y, aiPosition.z);
			data.timeStamp = timeStamp;
			myPositions.push_back(data);
		}

		myNumRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < myNumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			data.orientation = DirectX::XMFLOAT4(aiOrientation.x, aiOrientation.y, aiOrientation.z, aiOrientation.w);
			data.timeStamp = timeStamp;
			myRotations.push_back(data);
		}

		myNumScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < myNumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = (float)channel->mScalingKeys[keyIndex].mTime;
			KeyScale data;
			data.scale = DirectX::XMFLOAT3(scale.x, scale.y, scale.z);
			data.timeStamp = timeStamp;
			myScales.push_back(data);
		}
		myPositions;
		myRotations;
		myScales;

	}

	void Bone::Update(float animationTime, DirectX::XMMATRIX aPreRotation)
	{
		DirectX::XMMATRIX translation = DirectX::XMMatrixTranspose(InterpolatePosition(animationTime));

		DirectX::XMMATRIX rotation = DirectX::XMMatrixTranspose(InterpolateRotation(animationTime));
		DirectX::XMMATRIX scale = DirectX::XMMatrixTranspose(InterpolateScaling(animationTime));

		myLocalTransform = translation * aPreRotation * rotation * scale;

	}

	int Bone::GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < myNumPositions - 1; ++index)
		{
			if (animationTime < myPositions[index + 1].timeStamp)
				return index;
		}

		return 0;
	}

	int Bone::GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < myNumRotations - 1; ++index)
		{
			if (animationTime < myRotations[index + 1].timeStamp)
				return index;
		}

		return 0;

	}

	int Bone::GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < myNumScalings - 1; ++index)
		{
			if (animationTime < myScales[index + 1].timeStamp)
				return index;
		}

		return 0;

	}

	float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	DirectX::XMMATRIX Bone::InterpolatePosition(float animationTime)
	{
		if (1 == myNumPositions)
		{
			return DirectX::XMMatrixTranslation(myPositions[0].position.x, myPositions[0].position.y, myPositions[0].position.z);
		}
		int p0Index = GetPositionIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(myPositions[p0Index].timeStamp,
			myPositions[p1Index].timeStamp, animationTime);

		DirectX::XMVECTOR position0 = DirectX::XMLoadFloat3(&myPositions[p0Index].position);
		DirectX::XMVECTOR position1 = DirectX::XMLoadFloat3(&myPositions[p1Index].position);

		DirectX::XMVECTOR finalPosition = DirectX::XMVectorLerp(position0, position1, scaleFactor);

		return DirectX::XMMatrixTranslationFromVector(finalPosition);
	}

	DirectX::XMMATRIX Bone::InterpolateRotation(float animationTime)
	{
		if (1 == myNumRotations)
		{
			DirectX::XMVECTOR rotation = DirectX::XMLoadFloat4(&myRotations[0].orientation);
			rotation = DirectX::XMQuaternionNormalize(rotation);
			return DirectX::XMMatrixRotationQuaternion(rotation);
		}

		int p0Index = GetRotationIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(myRotations[p0Index].timeStamp,
			myRotations[p1Index].timeStamp, animationTime);

		DirectX::XMVECTOR rotation0 = DirectX::XMLoadFloat4(&myRotations[p0Index].orientation);
		DirectX::XMVECTOR rotation1 = DirectX::XMLoadFloat4(&myRotations[p1Index].orientation);

		rotation0 = DirectX::XMQuaternionNormalize(rotation0);
		rotation1 = DirectX::XMQuaternionNormalize(rotation1);

		DirectX::XMVECTOR finalRotation = DirectX::XMQuaternionSlerp(rotation0, rotation1, scaleFactor);
		finalRotation = DirectX::XMQuaternionNormalize(finalRotation);


		return DirectX::XMMatrixRotationQuaternion(finalRotation);
	}

	DirectX::XMMATRIX Bone::InterpolateScaling(float animationTime)
	{
		if (1 == myNumScalings)
			return DirectX::XMMatrixScaling(myScales[0].scale.x, myScales[0].scale.y, myScales[0].scale.z);

		int p0Index = GetScaleIndex(animationTime);
		int p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(myScales[p0Index].timeStamp, myScales[p1Index].timeStamp, animationTime);

		DirectX::XMVECTOR scale0 = DirectX::XMLoadFloat3(&myScales[p0Index].scale);
		DirectX::XMVECTOR scale1 = DirectX::XMLoadFloat3(&myScales[p1Index].scale);

		DirectX::XMVECTOR finalScale = DirectX::XMVectorLerp(scale0, scale1, scaleFactor);

		return DirectX::XMMatrixScalingFromVector(finalScale);
	}
}
