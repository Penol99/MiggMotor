#include "engine_pch.h"
#include <Animation.h>
#include <SkinnedModelComponent.h>
#include "Bone.h"
#include <cimport.h>
#include <Graphics.h>

namespace mi 
{
	Animation::Animation(std::string animationPath, SkinnedModelComponent* model)
	{

		aiPropertyStore* propertyStore = aiCreatePropertyStore();
		aiSetImportPropertyFloat(propertyStore, AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 1.0f);
		//aiSetImportPropertyInteger(propertyStore, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
		const C_STRUCT aiScene* scene = NULL;
		scene = aiImportFileExWithProperties(animationPath.c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GlobalScale, NULL, propertyStore);


		if (!scene)
		{
			ErrorLog::Log("Failed to load animation file: " + animationPath);
			return;
		}

		aiMatrix4x4 global = scene->mRootNode->mTransformation;
		global = global.Inverse();

		myGlobalInverse = ModelComponent::ConvertAiToXM(
			global
		);

		auto animation = scene->mAnimations[0];
		myTicksPerSecond = (float)animation->mTicksPerSecond;
		myDuration = (float)animation->mDuration;//((float)animation->mTicksPerSecond/((float)animation->mDuration*.1f))-1; // Seems neccesary for maya import, if it jitters make duration = mDuration only

		ReadHierarchyData(myRootNode, scene->mRootNode);
		ReadBones(animation, *model);

		aiReleaseImport(scene);

		aiReleasePropertyStore(propertyStore);

	}


	Bone* Animation::FindBone(const std::string& name)
	{
		auto iter = myBoneMap.find(name);
		if (iter != myBoneMap.end())
		{

			return iter->second;
		}
		else
		{
			return nullptr;
		}
	}

	void Animation::ReadBones(const aiAnimation* animation, SkinnedModelComponent& model)
	{
		int size = animation->mNumChannels;
		size;
		model;

		auto& boneInfoMap = model.GetBoneInfoMap();

		std::vector<Bone> translationBones;
		std::vector<Bone> rotationBones;
		std::vector<Bone> scaleBones;
		std::vector<Bone> combinedBones;
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string fullBoneName = channel->mNodeName.data;

			std::string boneName = fullBoneName.substr(0, fullBoneName.find("$") - 1);
			std::string type = fullBoneName.substr(fullBoneName.find("$_") + 2, fullBoneName.size() - 1);

			if (boneInfoMap.find(boneName) != boneInfoMap.end()) // BONE EXISTS
			{
				if (type._Equal("Translation"))
				{
					translationBones.push_back(Bone(fullBoneName,
						boneInfoMap[boneName].myID, channel));
				}
				else if (type._Equal("Rotation"))
				{
					rotationBones.push_back(Bone(boneName,
						boneInfoMap[boneName].myID, channel));
				}
				else if (type._Equal("Scaling"))
				{
					scaleBones.push_back(Bone(fullBoneName,
						boneInfoMap[boneName].myID, channel));
				}
				else
				{
					Bone fuckedBone = Bone(fullBoneName, boneInfoMap[boneName].myID, channel);
					std::vector<KeyPosition> positions = fuckedBone.GetPositions();
					std::vector<KeyScale> scales = fuckedBone.GetScales();

					combinedBones.push_back(fuckedBone);

				}
				continue;
			}
		}

		for (int i = 0; i < rotationBones.size(); i++)
		{
			if (translationBones.size() == rotationBones.size())
			{
				if (rotationBones[i].GetBoneID() == translationBones[i].GetBoneID())
				{
					Bone combinedBone = rotationBones[i];
					combinedBone.SetPositionVector(translationBones[i].GetPositions());

					combinedBones.push_back(combinedBone);
				}
			}
			else 
			{
				combinedBones = rotationBones;
			}
		}

		for (size_t i = 0; i < scaleBones.size(); i++)
		{
			for (size_t j = 0; j < combinedBones.size(); j++)
			{
				if (scaleBones[i].GetBoneID() == combinedBones[j].GetBoneID())
				{
					combinedBones[j].SetScaleVector(scaleBones[i].GetScales());
				}
			}
		}


		myBones = combinedBones;
		for (auto& bone : myBones)
		{
			
			myBoneMap[bone.GetBoneName()] = &bone;
		}
		myBoneInfoMap = boneInfoMap;
	}

	void Animation::ReadHierarchyData(AssimpNodeData& dest, aiNode* src)
	{
		assert(src);

		dest.name = src->mName.C_Str();

		dest.transformation = ModelComponent::ConvertAiToXM(src->mTransformation);

		dest.childrenCount = src->mNumChildren;

		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			newData.parent = &dest;
			ReadHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}



}