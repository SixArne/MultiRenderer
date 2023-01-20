#include "pch.h"
#include <vector>

#include "BaseRenderer.h"
#include "SDL.h"
#include "Camera.h"
#include "DataTypes.h"
#include "RenderConfig.h"


BaseRenderer::BaseRenderer(SDL_Window* pWindow, Camera* pCamera)
	:m_pWindow{ pWindow }, m_pCamera{ pCamera }
{

}

void BaseRenderer::Render()
{
	if (RENDER_CONFIG->ShouldUseUniformColor())
	{
		m_CurrentColor = m_UniformColor;
	}
	else
	{
		m_CurrentColor = m_RendererColor;
	}
}





