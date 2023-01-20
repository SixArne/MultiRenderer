//External includes
#include "pch.h"
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Texture.h"
#include "Utils.h"
#include "BaseRenderer.h"
#include "CPU_Renderer.h"
#include "DirectX_Renderer.h"
#include "Mesh.h"
#include "RenderConfig.h"

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	m_pCamera = new Camera{ };
	
	int w{}, h{};
	SDL_GetWindowSize(pWindow, &w, &h);

	m_pCamera->Initialize((float)w / (float)h, 45.f, {0,0,0});

	std::string vehicleFileLocation{ "Resources/vehicle.obj" };
	std::string fireFileLocation{ "Resources/fireFX.obj" };

	SetupVehicle(vehicleFileLocation);
	SetupThruster(fireFileLocation);

	LoadTextures();

	m_pCPURenderer = new CPU_Renderer(pWindow, m_pCamera, m_pMeshes);
	m_pDirectXRenderer = new DirectX_Renderer(pWindow, m_pCamera, m_pMeshes);
	m_pVulkanRenderer = new VulkanRenderer(pWindow, m_pCamera, m_pMeshes);

	m_pCurrentRenderer = m_pCPURenderer; 

	RENDER_CONFIG->PrintInstructions();
}

Renderer::~Renderer()
{
	for (MeshData* mesh : m_pMeshes)
	{
		delete mesh;
	}

	delete m_pCamera;
	delete m_pCPURenderer;
	delete m_pDirectXRenderer;
	delete m_pVulkanRenderer;
}

void Renderer::Update(Timer* pTimer)
{
	m_pCamera->Update(pTimer);

	m_pCurrentRenderer->Update(pTimer);
}

void Renderer::Render()
{
	auto api = RENDER_CONFIG->GetCurrentAPI();

	if (RENDER_CONFIG->GetShouldUseVulkan())
	{
		m_pCurrentRenderer = m_pVulkanRenderer;
	}
	else
	{
		switch (api)
		{
		case RenderConfig::API::DIRECTX:
			m_pCurrentRenderer = m_pDirectXRenderer;
			break;
		case RenderConfig::API::CPU:
			m_pCurrentRenderer = m_pCPURenderer;
			break;
		case RenderConfig::API::ENUM_LENGTH:
			throw std::runtime_error("Unknown API, bug in code");
		}
	}

	m_pCurrentRenderer->Render();
}

void Renderer::SetMoveSpeedFast()
{
	m_pCamera->SetMoveSpeedFast();
}

void Renderer::SetMoveSpeedNormal()
{
	m_pCamera->SetMoveSpeedNormal();
}

void Renderer::ToggleRotation()
{
	m_ShouldRotate = !m_ShouldRotate;
}

void Renderer::SetupVehicle(std::string& meshSrc)
{
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};

	Utils::ParseOBJ(meshSrc, vertices, indices);

	MeshData* mesh = new MeshData{};

	mesh->vertices = vertices;
	mesh->indices = indices;
	mesh->transformMatrix = Matrix::CreateTranslation({ 0,0,50.f });
	mesh->scaleMatrix = Matrix::CreateScale({ 1,1,1 });
	mesh->yawRotation = 90.f * TO_RADIANS;
	mesh->rotationMatrix = Matrix::CreateRotationY(mesh->yawRotation);
	mesh->worldMatrix = mesh->scaleMatrix * mesh->rotationMatrix * mesh->transformMatrix;

	mesh->texturesLocations[0] = "Resources/vehicle_diffuse.png";
	mesh->texturesLocations[1] = "Resources/vehicle_normal.png";
	mesh->texturesLocations[2] = "Resources/vehicle_specular.png";
	mesh->texturesLocations[3] = "Resources/vehicle_gloss.png";

	m_pMeshes.push_back(mesh);
}

void Renderer::SetupThruster(std::string& meshSrc)
{
	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};

	Utils::ParseOBJ(meshSrc, vertices, indices);

	MeshData* mesh = new MeshData{};

	mesh->vertices = vertices;
	mesh->indices = indices;
	mesh->transformMatrix = Matrix::CreateTranslation({ 0,0,50.f });
	mesh->scaleMatrix = Matrix::CreateScale({ 1,1,1 });
	mesh->yawRotation = 90.f * TO_RADIANS;
	mesh->rotationMatrix = Matrix::CreateRotationY(mesh->yawRotation);
	mesh->worldMatrix = mesh->scaleMatrix * mesh->rotationMatrix * mesh->transformMatrix;

	mesh->texturesLocations[0] = "Resources/fireFX_diffuse.png";

	m_pMeshes.push_back(mesh);
}

void Renderer::LoadTextures()
{
	for (MeshData* mesh : m_pMeshes)
	{
		int textureIndex{};

		for (const std::string& textureLocation : mesh->texturesLocations)
		{
			if (!textureLocation.empty())
			{
				mesh->textures[textureIndex++] = Texture::LoadFromFile(textureLocation);
			}	
		}
	}
}
