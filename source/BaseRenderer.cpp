#include "pch.h"
#include <vector>

#include "BaseRenderer.h"
#include "SDL.h"
#include "Camera.h"
#include "DataTypes.h"


BaseRenderer::BaseRenderer(SDL_Window* pWindow, Camera* pCamera)
	:m_pWindow{ pWindow }, m_pCamera{ pCamera }
{

}




