#pragma once
#include <vector>
#include "Camera.h"
#include "DataTypes.h"
#include <unordered_map>

class SDL_Window;


class BaseRenderer
{
public:
	BaseRenderer(SDL_Window* pWindow, Camera* pCamera);
	virtual ~BaseRenderer() = default;

	BaseRenderer(const BaseRenderer&) = delete;
	BaseRenderer(BaseRenderer&&) noexcept = delete;
	BaseRenderer& operator=(const BaseRenderer&) = delete;
	BaseRenderer& operator=(BaseRenderer&&) noexcept = delete;

	virtual void Update(Timer* pTimer) {};
	virtual void Render();

protected:
	SDL_Window* m_pWindow{};

	int m_Width{};
	int m_Height{};

	ColorRGB m_RendererColor{};
	ColorRGB m_UniformColor{};
	ColorRGB m_CurrentColor{};

	bool m_IsUsingUniformColor{ false };

	Camera* m_pCamera;
};

