#include "Engine.h"
#include <Windows.h>
#include "Timer.h"

#include "PropertyDefines.h"
#include "GameWorld.h"

Timer Time;
void Run(void);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	nCmdShow;
	lpCmdLine;
	hPrevInstance;
	hInstance;
	// Open Console Window
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);


	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLog::Log(hr, "failed calling CoInitialize");
		return -1;
	}

	mi::Engine& engine = *mi::Engine::GetInstance();
	if(engine.Init(hInstance, "MiggEngine", "Window", ENGINE_WINDOW_W, ENGINE_WINDOW_H,Time))
	{
		Run();
	}
	return 0;
}

void Run()
{
	

	mi::Engine& engine = *mi::Engine::GetInstance();
	mi::GameWorld game(engine);
	
	game.Init();
	bool gameRunning = true;

	while (engine.ProcessMessages() && gameRunning)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		Time.Update();
		engine.Update(Time.GetDeltaTime());

		if (!game.Update())
		{
			gameRunning = false;
		}
		game.Render();
		game.RenderTools();
		ImGui::Render();
		ImGui::EndFrame();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		engine.EndFrame();
	}

}

