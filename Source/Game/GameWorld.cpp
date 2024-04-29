#include "GameWorld.h"
#include "Input/InputHandler.h"
#include <GameObject.h>
#include "Engine.h"
#include <FFTWaterSimulation.h>
namespace mi
{
	GameWorld::GameWorld(Engine& anEngine) :
		myEngine(anEngine)
	{

	}

	GameWorld::~GameWorld()
	{
		for (auto& g : myGameObjects)
		{
			delete g;
		}
	}


	void GameWorld::Init()
	{
		myWaterObject = new GameObject("WaterObject");

		const int waterSize = 256;
		myWaterSimComponent = myWaterObject->AddComponent<FFTWaterSimulation>();
		myWaterSimComponent->Init(waterSize);
		myGameObjects.push_back(myWaterObject);
	}


	bool GameWorld::Update()
	{
		for (auto& g : myGameObjects)
		{
			g->Update(myEngine.GetTimer().GetDeltaTime());
		}
		return true;
	}

	void GameWorld::Render()
	{
		Engine::GetInstance()->Render(myGameObjects);
	}
	void GameWorld::RenderTools()
	{
		myWaterSimComponent->RenderTool();
	}
}
