#pragma once
#include <vector>
#include <memory>


namespace mi
{
	class GameObject;
	class Engine;
	class FFTWaterSimulation;

	class GameWorld
	{
	public:
		GameWorld() = delete;
		GameWorld(Engine& anEngine);
		~GameWorld();
		void Init();
		bool Update();
		void Render();
		void RenderTools();
	private:

	private:
		Engine& myEngine;
		
		std::vector<GameObject*> myGameObjects;
		GameObject* myWaterObject;
		FFTWaterSimulation* myWaterSimComponent;
	};
}
