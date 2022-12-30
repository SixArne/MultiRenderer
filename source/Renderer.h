#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"
#include "VulkanRenderer.h"

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

	Renderer(SDL_Window* pWindow);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void Update(Timer* pTimer);
	void Render();
	void SetMoveSpeedFast();
	void SetMoveSpeedNormal();
	void ToggleRotation();

private:
	SDL_Window* m_pWindow{};
	CPU_Renderer* m_pCPURenderer{};
	DirectX_Renderer* m_pDirectXRenderer{};
	VulkanRenderer* m_pVulkanRenderer{};

	BaseRenderer* m_pCurrentRenderer{};
	Camera* m_pCamera{};

	// List of meshes
	std::vector<MeshData*> m_pMeshes{};

	void SetupVehicle(std::string& meshSrc);
	void SetupThruster(std::string& meshSrc);
	void LoadTextures();

	int m_Width{};
	int m_Height{};

	bool m_ShouldRotate{ true };
};
