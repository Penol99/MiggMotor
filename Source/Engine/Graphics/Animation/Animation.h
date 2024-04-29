#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <DirectXMath.h>
#include <scene.h>
#include <Importer.hpp>
#include <postprocess.h>
#include <unordered_map>
#include "Bone.h"

namespace mi 
{
	class SkinnedModelComponent;
	struct BoneInfo;
	class Bone;
	struct AssimpNodeData
	{
		DirectX::XMMATRIX transformation;
		std::string name;
		int childrenCount;
		std::vector<AssimpNodeData> children;
		AssimpNodeData* parent;
	};

	class Animation
	{
	public:
		Animation() = default;

		Animation(std::string animationPath, SkinnedModelComponent* model);


		~Animation()
		{
		}

		Bone* FindBone(const std::string& name);


		inline float GetTicksPerSecond() { return myTicksPerSecond; }

		inline float GetDuration() { return myDuration; }

		inline const AssimpNodeData& GetRootNode() { return myRootNode; }

		inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return myBoneInfoMap; }

	private:
		void ReadBones(const aiAnimation* animation, SkinnedModelComponent& model);


		void ReadHierarchyData(AssimpNodeData& dest, aiNode* src);
		float myDuration;
		float myTicksPerSecond;
		std::vector<Bone> myBones;
		AssimpNodeData myRootNode;
		DirectX::XMMATRIX myGlobalInverse;
		std::map<std::string, BoneInfo> myBoneInfoMap;
		std::unordered_map<std::string, Bone*> myBoneMap;

		friend class AnimationPlayerComponent;
	};
}