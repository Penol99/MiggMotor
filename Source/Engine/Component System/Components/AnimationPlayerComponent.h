#pragma once

#include <Component.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <map>
#include <Vector3.h>


namespace mi
{
	namespace MODELVIEWER
	{
		class ModelScene;
	}
	
	struct AssimpNodeData;
	class SkinnedModelComponent;
	class Animation;
	class AnimationPlayerComponent : public Component
	{
	public:
		AnimationPlayerComponent();

		void Init() override;
		void Update(const float dt);

		void PlayAnimation(const std::string aName,bool loop = false);
		void PlayAnimationBlended(const std::string aNameOne, const std::string aNameTwo, bool loop);

		float myBlendValue;

	
#undef GetCurrentTime
		float GetCurrentTime() const;
		float GetCurrentTimelinePercentage() const;
		float GetCurrentDuration() const;
		float GetCurrentTicksPerSecond() const;
		float GetDuration(const std::string& aName) const;
		float GetTicksPerSecond(const std::string& aName) const;

		bool HasAnimation(const std::string& aName) const;
		void SetSpeedMultiplier(const float aSpeedMultiplier);

	
		void CreateDirectoriesRecursively(const std::string& directory);


		void ReadBakedDataFromFile(const std::string filename, const std::string aNameForTheAnimation);

		void BakeAnimation(Animation animation, std::string aFileName);

		void CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX parentTransform, Animation* animation);

		std::vector<DirectX::XMMATRIX> GetFinalBoneMatrices();

		inline void SetLooping(bool aValue) { myAnimationIsLooping = aValue; }
		inline bool IsLooping() const { return myAnimationIsLooping; }
	private:
		void Update(const float dt, float blendValue);

		void WriteBakedDataToFile(const std::string filename, float myDuration, float myTicksPerSecond, std::vector<std::vector<DirectX::XMMATRIX>>& aBoneMatric);

		std::vector<DirectX::XMMATRIX> myFinalBoneMatrices;
		std::vector<std::vector<DirectX::XMMATRIX>>* myBakedBoneMatrices = nullptr;
		std::vector<std::vector<DirectX::XMMATRIX>>* myNextAnimationBakedBoneMatrices = nullptr;

		std::map<std::string, std::vector<std::vector<DirectX::XMMATRIX>>> myBakedAnimations;
		std::vector<std::vector<DirectX::XMMATRIX>> myBlendedAnimation;
		std::map<std::string, float> myBakedTicksPerSecond;
		std::map<std::string, float> myBakedDuration;

		SkinnedModelComponent* myModel;
		std::map<std::string, DirectX::XMMATRIX> myPreRotationMap;
		float myCurrentTime;
		float myDeltaTime;

		float myCurrentTicksPerSecond;
		float myCurrentDuration;
		bool myAnimationIsLooping;
		float mySpeedMultiplier;
		float myBlendTime;
		float myBlendDuration;

		float myNextAnimationCurrentTime;
		float myNextAnimationDuration;
		float myNextAnimationTicksPerSecond;

		bool myShouldBlend = false;
		friend class MODELVIEWER::ModelScene;
		
	};
}