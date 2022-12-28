#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"
#include "RenderConfig.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	// 640 480
	const uint32_t width = 1920;
	const uint32_t height = 1080;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - Six Arne 2DAE08",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				//Test for a key
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					RENDER_CONFIG->CycleRenderer();
					pRenderer->SwitchRenderer();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					RENDER_CONFIG->ToggleRotation();
					//pRenderer->ToggleRotation();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
				{
					pRenderer->SetMoveSpeedNormal();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					RENDER_CONFIG->CycleCullMode();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					RENDER_CONFIG->ToggleUniformColor();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					RENDER_CONFIG->TogglePrintFPS();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F3)
				{
					RENDER_CONFIG->ToggleThruster();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					RENDER_CONFIG->CycleSamplingStates();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F5)
				{
					RENDER_CONFIG->CycleShadingMode();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F6)
				{
					RENDER_CONFIG->ToggleNormapMap();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F7)
				{
					RENDER_CONFIG->ToggleDepthBuffer();
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F8)
				{
					RENDER_CONFIG->ToggleBoundingBox();
				}

				break;

			case SDL_KEYDOWN:
				if (e.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
				{
					pRenderer->SetMoveSpeedFast();
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	// Destroy instance
	RENDER_CONFIG->DestroyInstance();

	ShutDown(pWindow);
	return 0;
}