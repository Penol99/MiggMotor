#pragma once

#include "Components/ModelComponent.h"
#include "Transform.h"
#include <Camera.h>
#include <Timer.h>
#include <wrl/client.h>
#include <vector>
#include <InputHandler.h>
using Microsoft::WRL::ComPtr;

namespace mi
{


	class GameObject
	{
	public:

		const int myID;
		std::string myName;

		std::vector<Component*> myComponents;
		GameObject* myParent;
		std::vector<GameObject*> myChildren;
		Transform myTransform;
		bool myIsActive;

		GameObject() = default;
		GameObject(const int anID)
			: myParent(nullptr)
			, myID(anID)
			, myName("Not defined")
			, myIsActive(true)
		{
		}
		GameObject(const std::string& aName)
			: myParent(nullptr)
			, myName(aName)
			, myID(-1)
			, myIsActive(true)
		{
		}


		template <typename T>
		T* AddComponent()
		{
			static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

			T* object = new T();
			object->myGameObject = this;
			myComponents.push_back(object);
			return object;
		}
		void AddComponent(Component* aComponent)
		{
			aComponent->myGameObject = this;
			myComponents.push_back(aComponent);
		}
		template <typename T>
		T* GetComponent()
		{
			for (Component* component : myComponents)
			{
				if (T* castComponent = dynamic_cast<T*>(component))
				{
					return castComponent;
				}
			}
			return nullptr;
		}

		void SetParent(GameObject* parent)
		{
			myParent = parent;
		}

		GameObject* GetParent() const
		{
			return myParent;
		}

		const std::vector<GameObject*>& GetChildren() const
		{
			return myChildren;
		}

		void AddChild(GameObject* child)
		{
			child->SetParent(this);
			myChildren.push_back(child);
		}

		void RemoveChild(GameObject* child)
		{
			auto it = std::find(myChildren.begin(), myChildren.end(), child);
			if (it != myChildren.end())
			{
				(*it)->SetParent(nullptr);
				myChildren.erase(it);
			}
		}

		~GameObject()
		{
			for (auto o : myComponents)
			{
				delete o;
			}
		};


		void Init()
		{
			for (auto o : myComponents)
			{
				o->myGameObject = this;
				o->Init();
			}
		}

		void Update(const float aDeltaTime)
		{
			if (!myIsActive) return;
			myTransform.Update(myParent ? &myParent->myTransform : nullptr);
			for (auto o : myComponents)
			{
				o->Update(aDeltaTime);
			}
		}

		void Render()
		{
			if (!myIsActive) return;

			for (auto o : myComponents)
			{
				o->Render();
			}
		}


	private:
	};
}