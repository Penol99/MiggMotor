#include "engine_pch.h"
#include "AnimationPlayerComponent.h"
#include "Animation.h"
#include "Bone.h"
#include "SkinnedModelComponent.h"
#include <fstream>
#include <StringConverter.h>
#include <GameObject.h>
#include "ErrorLog.h"



namespace mi
{
	AnimationPlayerComponent::AnimationPlayerComponent() {}

	void AnimationPlayerComponent::Init()
	{
		myCurrentTime = 0.0;
		myNextAnimationCurrentTime = 0;

		myModel = myGameObject->GetComponent<SkinnedModelComponent>();
		myFinalBoneMatrices.resize(MAX_ANIMATION_BONES, DirectX::XMMatrixIdentity());
	}

	void AnimationPlayerComponent::Update(const float dt)
	{
		
		if (myBakedBoneMatrices != nullptr)
		{
			if (myShouldBlend)
			{
				Update(dt, myBlendValue);
				return;
			}
			myDeltaTime = dt * mySpeedMultiplier;
			myCurrentTime += myCurrentTicksPerSecond * myDeltaTime;
			if (myAnimationIsLooping)
			{
				myCurrentTime = fmod(myCurrentTime, myCurrentDuration);
			}
			else if (myCurrentTime >= myCurrentDuration)
			{
				myCurrentTime = myCurrentDuration;
			}

			float frameIndexFloat = myCurrentTime * myCurrentTicksPerSecond;
			int frameIndex = static_cast<int>(floor(frameIndexFloat));
			if (frameIndex >= myBakedBoneMatrices->size())
			{
				frameIndex = (int)myBakedBoneMatrices->size() - 1;
			}

			for (int i = 0; i < MAX_ANIMATION_BONES; ++i)
			{
				myFinalBoneMatrices[i] = (*myBakedBoneMatrices)[frameIndex][i];
			}

			memcpy(myModel->myBoneTransforms, myFinalBoneMatrices.data(), myFinalBoneMatrices.size() * sizeof(myFinalBoneMatrices[0]));
		}
		else
		{

			std::vector<DirectX::XMMATRIX> empty(MAX_ANIMATION_BONES, DirectX::XMMatrixIdentity());
			memcpy(myModel->myBoneTransforms, empty.data(), MAX_ANIMATION_BONES * sizeof(DirectX::XMMatrixIdentity()));

		}
	}

	bool DecomposeMatrix(const DirectX::XMMATRIX& matrix, DirectX::XMVECTOR& scale, DirectX::XMVECTOR& rotation, DirectX::XMVECTOR& translation) {
		return DirectX::XMMatrixDecompose(&scale, &rotation, &translation, matrix);
	}
	DirectX::XMVECTOR Lerp(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end, float percent) {
		return DirectX::XMVectorLerp(start, end, percent);
	}
	DirectX::XMVECTOR Slerp(const DirectX::XMVECTOR& start, const DirectX::XMVECTOR& end, float percent) {
		return DirectX::XMQuaternionSlerp(start, end, percent);
	}
	DirectX::XMMATRIX InterpolateMatrices(const DirectX::XMMATRIX& startMatrix, const DirectX::XMMATRIX& endMatrix, float percent) {
		
		DirectX::XMVECTOR startScale, startRotation, startTranslation;
		DirectX::XMVECTOR endScale, endRotation, endTranslation;


		DecomposeMatrix(DirectX::XMMatrixTranspose(startMatrix), startScale, startRotation, startTranslation);
		DecomposeMatrix(DirectX::XMMatrixTranspose(endMatrix), endScale, endRotation, endTranslation);

		DirectX::XMVECTOR interpScale = Lerp(startScale, endScale, percent);
		DirectX::XMVECTOR interpTranslation = Lerp(startTranslation, endTranslation, percent);
		DirectX::XMVECTOR interpRotation = Slerp(startRotation, endRotation, percent);

		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(interpScale);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(interpRotation);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(interpTranslation);


		return DirectX::XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix);
	}

	void AnimationPlayerComponent::Update(const float dt, float blendValue)
	{

		myDeltaTime = dt * mySpeedMultiplier;

		if (myBakedBoneMatrices != nullptr)
		{
			myCurrentTime += myCurrentTicksPerSecond * myDeltaTime;
			if (myAnimationIsLooping)
			{
				myCurrentTime = fmod(myCurrentTime, myCurrentDuration);

			}
			else if (myCurrentTime >= myCurrentDuration)
			{
				myCurrentTime = myCurrentDuration;
			}


			float frameIndexFloat = myCurrentTime * myCurrentTicksPerSecond;
			int frameIndex = static_cast<int>(floor(frameIndexFloat));
			frameIndex;
			//frameIndex = frameIndex >= myBakedBoneMatrices->size() ? (int)myBakedBoneMatrices->size() - 1 : frameIndex;

			if (myNextAnimationBakedBoneMatrices != nullptr)
			{

				myNextAnimationCurrentTime += myNextAnimationTicksPerSecond * myDeltaTime;
				if (myAnimationIsLooping)
				{
					myNextAnimationCurrentTime = fmod(myNextAnimationCurrentTime, myNextAnimationDuration);
				}
				else if (myNextAnimationCurrentTime >= myNextAnimationDuration)
				{
					myNextAnimationCurrentTime = myNextAnimationDuration;
				}
				//myBlendTime += dt;
				//float blendFactor = myBlendTime / myBlendDuration;
				//blendFactor = blendFactor > 1.0f ? 1.0f : blendFactor;


				float nextFrameIndexFloat = myNextAnimationCurrentTime * myNextAnimationTicksPerSecond;
				int nextFrameIndex = static_cast<int>(floor(nextFrameIndexFloat));
				nextFrameIndex;
				//nextFrameIndex = nextFrameIndex >= myNextAnimationBakedBoneMatrices->size() ? (int)myNextAnimationBakedBoneMatrices->size() - 1 : nextFrameIndex;

				for (int i = 0; i < MAX_ANIMATION_BONES; ++i)
				{
					
					myFinalBoneMatrices[i] = InterpolateMatrices((*myBakedBoneMatrices)[frameIndex][i], (*myNextAnimationBakedBoneMatrices)[nextFrameIndex][i], blendValue);
					//myFinalBoneMatrices[i] = (*myBakedBoneMatrices)[frameIndex][i];
					//myFinalBoneMatrices[i] = (*myNextAnimationBakedBoneMatrices)[nextFrameIndex][i];
				}

				//if (blendFactor >= 1.0f)
				//{
				//	blendFactor = 0.f;
				//	myBlendTime = 0.f;
				//	//blend = false;
				//	//std::swap(myBakedBoneMatrices, myNextAnimationBakedBoneMatrices);
				//	//myCurrentTime = myNextAnimationCurrentTime;
				//	//myCurrentDuration = myNextAnimationDuration;
				//	//myCurrentTicksPerSecond = myNextAnimationTicksPerSecond;
				//}
			}

			memcpy(myModel->myBoneTransforms, myFinalBoneMatrices.data(), myFinalBoneMatrices.size() * sizeof(myFinalBoneMatrices[0]));
		}
		else
		{
			std::vector<DirectX::XMMATRIX> empty(MAX_ANIMATION_BONES, DirectX::XMMatrixIdentity());
			memcpy(myModel->myBoneTransforms, empty.data(), MAX_ANIMATION_BONES * sizeof(DirectX::XMMatrixIdentity()));
		}
	}


	void AnimationPlayerComponent::PlayAnimation(const std::string aName, bool loop)
	{
		myShouldBlend = false;
		assert(HasAnimation(aName) && std::string("Animation does not exist: " + aName).c_str());
		myBakedBoneMatrices = &myBakedAnimations.at(aName);
		myCurrentTicksPerSecond = myBakedTicksPerSecond.at(aName);
		myCurrentDuration = myBakedDuration.at(aName) - 1; // Seems to fuck up if i dont exclude last frame as last frame seems to be garbage;
		myCurrentTime = 0.0f;
		myAnimationIsLooping = loop;
		mySpeedMultiplier = 1.f;
	}

	void AnimationPlayerComponent::PlayAnimationBlended(const std::string aNameOne, const std::string aNameTwo, bool loop)
	{
		myShouldBlend = true;
		assert(HasAnimation(aNameOne) && std::string("Animation does not exist: " + aNameOne).c_str());
		assert(HasAnimation(aNameTwo) && std::string("Animation does not exist: " + aNameTwo).c_str());

		myBakedBoneMatrices = &myBakedAnimations.at(aNameOne);
		myCurrentTicksPerSecond = myBakedTicksPerSecond.at(aNameOne);
		myCurrentDuration = myBakedDuration.at(aNameOne) - 1; 
		myCurrentTime = 0.0f;

		myNextAnimationBakedBoneMatrices = &myBakedAnimations.at(aNameTwo);
		myNextAnimationTicksPerSecond = myBakedTicksPerSecond.at(aNameTwo);
		myNextAnimationDuration = myBakedDuration.at(aNameTwo) - 1;
		myNextAnimationCurrentTime = 0.0f;

		myBlendTime = 0.f;
		myBlendDuration = 1.f;

		myAnimationIsLooping = loop;
		mySpeedMultiplier = 1.f;
	}






	


	//AnimationManager
	//std::map<Type,map<std::string, std::vector<std::vector<DirectX::XMMATRIX>>>> AnimationMap 
	//
	//
	//

	//animPlayComponent->PlayAnimation(eEnemy1,"Attack") {
	//myBakedBoneMatricies = AnimManager.Get(eEnemy1,"Attack");
	//}
	float AnimationPlayerComponent::GetCurrentTime() const
	{
		return myCurrentTime;
	}

	float AnimationPlayerComponent::GetCurrentTimelinePercentage() const
	{
		return myCurrentTime / myCurrentDuration;
	}

	float AnimationPlayerComponent::GetCurrentDuration() const
	{
		return myCurrentDuration;
	}

	float AnimationPlayerComponent::GetCurrentTicksPerSecond() const
	{
		return myCurrentTicksPerSecond;
	}

	float AnimationPlayerComponent::GetDuration(const std::string& aName) const
	{
		return myBakedDuration.at(aName);
	}

	float AnimationPlayerComponent::GetTicksPerSecond(const std::string& aName) const
	{
		return myBakedTicksPerSecond.at(aName);
	}

	bool AnimationPlayerComponent::HasAnimation(const std::string& aName) const
	{
		return myBakedAnimations.find(aName) != myBakedAnimations.end();
	}

	void AnimationPlayerComponent::SetSpeedMultiplier(const float aSpeedMultiplier)
	{
		mySpeedMultiplier = aSpeedMultiplier;
	}

	void AnimationPlayerComponent::CreateDirectoriesRecursively(const std::string& directory) // Does not entirely work as intended, does not create the directries recursively, can only create one folder
	{
		std::string::size_type slash = directory.find_first_of("/\\");
		while (slash != std::string::npos)
		{
			std::string dir = directory.substr(0, slash);
			if (!CreateDirectory(StringConverter::StringToWide(dir).c_str(), NULL))
			{
				DWORD error = GetLastError();
				if (error != ERROR_ALREADY_EXISTS)
				{
					ErrorLog::Log("Failed to create directory: " + dir + ", Error: " + std::to_string(error));
				}
			}
			slash = directory.find_first_of("/\\", slash + 1);
		}
	}


	void AnimationPlayerComponent::WriteBakedDataToFile(const std::string filename, float myDuration, float myTicksPerSecond, std::vector<std::vector<DirectX::XMMATRIX>>& aBoneMatric)
	{
		std::string directory = filename.substr(0, filename.find_last_of("/\\"));
		CreateDirectoriesRecursively(directory);

		std::ofstream outFile(filename, std::ios::binary | std::ios::out);

		if (!outFile.is_open())
		{
			ErrorLog::Log("failed opening/creating new baked animation file");
			return;
		}

		// Write myDuration and myTicksPerSecond to the file before the matrices.
		outFile.write(reinterpret_cast<const char*>(&myDuration), sizeof(float));
		outFile.write(reinterpret_cast<const char*>(&myTicksPerSecond), sizeof(float));

		// Write the matrices
		for (const auto& frame : aBoneMatric)
		{
			for (const auto& matrix : frame)
			{
				outFile.write(reinterpret_cast<const char*>(&matrix), sizeof(DirectX::XMMATRIX));
			}
		}

		outFile.close();
	}


	void AnimationPlayerComponent::ReadBakedDataFromFile(const std::string filename, const std::string aNameForTheAnimation)
	{
		myCurrentTime = 0.0;
		myFinalBoneMatrices.resize(MAX_ANIMATION_BONES, DirectX::XMMatrixIdentity());
		std::ifstream inFile(filename, std::ios::binary | std::ios::in);

		if (!inFile.is_open())
		{
			ErrorLog::Log("failed opening baked animation file");
			return;
		}

		inFile.read(reinterpret_cast<char*>(&myCurrentDuration), sizeof(float));
		inFile.read(reinterpret_cast<char*>(&myCurrentTicksPerSecond), sizeof(float));
		myBakedDuration.emplace(aNameForTheAnimation, myCurrentDuration);
		myBakedTicksPerSecond.emplace(aNameForTheAnimation, myCurrentTicksPerSecond);
		inFile.seekg(0, std::ios::end);
		std::streampos end = inFile.tellg();
		inFile.seekg(sizeof(float) * 2, std::ios::beg);

		std::vector<std::vector<DirectX::XMMATRIX>> bakedTempBones;
		while (inFile.tellg() < end)
		{
			std::vector<DirectX::XMMATRIX> frame;
			frame.reserve(MAX_ANIMATION_BONES);

			for (int i = 0; i < MAX_ANIMATION_BONES; ++i)
			{
				DirectX::XMMATRIX matrix;
				if (inFile.read(reinterpret_cast<char*>(&matrix), sizeof(DirectX::XMMATRIX)).gcount() < sizeof(DirectX::XMMATRIX))
				{
					ErrorLog::Log("Error reading matrix from baked animation file");
					break;
				}
				frame.push_back(matrix);
			}
			if (!frame.empty())
			{
				bakedTempBones.push_back(frame);
			}
		}

		inFile.close();
		myBakedAnimations.emplace(aNameForTheAnimation, bakedTempBones);
	}

	void AnimationPlayerComponent::BakeAnimation(Animation animation, std::string aFileName)
	{
		std::vector<std::vector<DirectX::XMMATRIX>> bakedTempBones;
		myFinalBoneMatrices.resize(MAX_ANIMATION_BONES, DirectX::XMMatrixIdentity());

		for (float time = 0.0f; time < animation.GetDuration(); time += 1.0f / animation.GetTicksPerSecond())
		{
			myCurrentTime = time;
			CalculateBoneTransform(&animation.GetRootNode(), DirectX::XMMatrixIdentity(), &animation);
			bakedTempBones.push_back(myFinalBoneMatrices);
		}
		WriteBakedDataToFile(aFileName, animation.GetDuration(), animation.GetTicksPerSecond(), bakedTempBones);
	}




	void AnimationPlayerComponent::CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform, Animation* animation)
	{
		std::string nodeName = node->name;
		std::string boneName = nodeName.substr(0, nodeName.find("$") - 1);
		std::string type = nodeName.substr(nodeName.find("$_") + 2, nodeName.size() - 1);
		DirectX::XMMATRIX nodeTransform = node->transformation;

		Bone* bone = animation->FindBone(nodeName);
		if (type._Equal("PreRotation"))
		{
			if (animation->FindBone(boneName))
			{
				nodeTransform = DirectX::XMMatrixIdentity();
			}

			myPreRotationMap.emplace(boneName, node->transformation);
		}
		if (bone)
		{
			bone->Update(myCurrentTime,
				// If Prerotation exists, add prerotation, else identity matrix
				(myPreRotationMap.find(boneName) != myPreRotationMap.end())
				? myPreRotationMap[boneName]
				: DirectX::XMMatrixIdentity());

			nodeTransform = bone->GetLocalTransform();

		}
		if (animation->FindBone(boneName) != nullptr && (type._Equal("Translation") || type._Equal("Rotation")))
		{
			nodeTransform = DirectX::XMMatrixIdentity();
		}


		DirectX::XMMATRIX globalTransformation = parentTransform * nodeTransform;

		auto& boneInfoMap = animation->GetBoneIDMap();

		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap.at(nodeName).myID;
			//DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
			DirectX::XMMATRIX offset = boneInfoMap.at(nodeName).offset;

			myFinalBoneMatrices[index] = animation->myGlobalInverse * globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation, animation);
		}
	}

	std::vector<DirectX::XMMATRIX> AnimationPlayerComponent::GetFinalBoneMatrices()
	{
		return myFinalBoneMatrices;
	}
}