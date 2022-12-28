#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;


class Texture;
class MeshData;
struct Vertex;
class Timer;
class BaseRenderer;
class CPU_Renderer;
class DirectX_Renderer;

class Renderer final
{
public:
	enum class API
	{
		CPU,
		DirectX,
		ENUM_LENGTH
	};

	Renderer(SDL_Window* pWindow);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void Update(Timer* pTimer);
	void Render();
	void SwitchRenderer();

private:
	SDL_Window* m_pWindow{};
	CPU_Renderer* m_pCPURenderer{};
	DirectX_Renderer* m_pDirectXRenderer{};

	BaseRenderer* m_pCurrentRenderer{};
	Camera* m_pCamera{};

	// List of meshes
	std::vector<MeshData*> m_pMeshes{};

	API m_CurrentAPI{ API::CPU };

	void SetupVehicle(std::string& meshSrc);
	void SetupThruster(std::string& meshSrc);
	void LoadTextures();

	int m_Width{};
	int m_Height{};
};
