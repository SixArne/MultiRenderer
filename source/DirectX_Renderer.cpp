#include "pch.h"
#include "DirectX_Renderer.h"

#include "Camera.h"
#include <SDL.h>
#include "DataTypes.h"
#include <vector>
#include "Utils.h"
#include <iostream>
#include "Mesh.h"
#include "DirectXMesh.h"
#include "VehicleEffect.h"
#include "ThrusterEffect.h"
#include "RenderConfig.h"

DirectX_Renderer::DirectX_Renderer(SDL_Window* pWindow, Camera* pCamera, std::vector<MeshData*> pMeshes)
	: BaseRenderer(pWindow, pCamera)
{
	m_RendererColor = ColorRGB{ 0.39f, 0.59f, 0.93f };
	m_UniformColor = ColorRGB{ 0.1f, 0.1f, 0.1f };
	m_CurrentColor = m_RendererColor;

	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Initialize DirectX pipeline
	const HRESULT result = InitializeDirectX();
	if (result == S_OK)
	{
		m_IsInitialized = true;
		std::cout << "DirectX is initialized and ready!\n";
	}
	else
	{
		std::cout << "DirectX initialization failed!\n";
	}

	CreateMeshes(pMeshes);
	CreateTextureSurfaces(pMeshes);

	BindTextures();

	auto lightDir = Vector3{ 0.577f, -0.577f, 0.577f };

	for (auto* mesh : m_pMeshes)
	{
		mesh->SetLightDirection(lightDir);
	}
}

DirectX_Renderer::~DirectX_Renderer()
{
	if (m_pDeviceContext)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();

		m_pDeviceContext->ClearState();
		m_pDeviceContext->Flush();
		m_pDeviceContext->Release();

		m_pDevice->Release();
	}

	for (auto* mesh : m_pMeshes)
	{
		delete mesh;
	}
}

void DirectX_Renderer::Update(Timer* pTimer)
{
	if (RENDER_CONFIG->ShouldRotate())
	{
		for (DirectXMesh* directXMesh : m_pMeshes)
		{
			MeshData* mesh = directXMesh->GetMeshData();
			
			// 1 deg per second
			const float degreesPerSecond = RENDER_CONFIG->GetRotationSpeed();
			mesh->AddRotationY((degreesPerSecond * pTimer->GetElapsed()) * TO_RADIANS);
		}
	}

	for (DirectXMesh* directXMesh : m_pMeshes)
	{
		directXMesh->GetEffect()->UpdateCullModes();
		directXMesh->GetEffect()->UpdateSamplerState();
	}
}

void DirectX_Renderer::Render()
{
	// call to base render function for uniform color changes
	BaseRenderer::Render();

	if (!m_IsInitialized)
		return;

	// Clear RTV and DSV
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &m_CurrentColor.r);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Setup pipeline + invoke draw calls
	
	int thrusterIndex = 1;

	for (int i{}; i < m_pMeshes.size(); i++)
	{
		if (!RENDER_CONFIG->ShouldRenderThruster() && i == thrusterIndex)
		{
			break;
		}

		DirectXMesh* mesh = m_pMeshes[i];

		auto meshData = mesh->GetMeshData();

		Matrix inverseView = m_pCamera->GetViewInverseMatrix();
		Matrix worldMatrix = meshData->worldMatrix;
		Matrix projMatrix = m_pCamera->GetProjectionMatrix();
		Matrix wvp = worldMatrix * m_pCamera->GetViewMatrix() * m_pCamera->GetProjectionMatrix();

		mesh->SetWorldMatrix(worldMatrix);
		mesh->SetViewInverse(inverseView);
		mesh->Render(m_pDeviceContext, wvp);
	}
	
	// Present back buffer
	m_pSwapChain->Present(0, 0);
}

HRESULT DirectX_Renderer::InitializeDirectX()
{
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	uint32_t createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create device to interface with D11
	HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
	if (FAILED(result))
	{
		throw std::runtime_error("Error creating device");
		return result;
	}

	// Setup swapchain
	IDXGIFactory1* pDxgiFactory{};
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
	if (FAILED(result))
	{
		throw std::runtime_error("Error creating factory");
		return result;
	}

	// Create swapchain info struct
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = m_Width;
	swapChainDesc.BufferDesc.Height = m_Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// Get handle (HWND) from the SDL Back buffer
	SDL_SysWMinfo sysWMInfo{};
	SDL_VERSION(&sysWMInfo.version);
	SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
	swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

	result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
	if (FAILED(result))
	{
		throw std::runtime_error("Error creating swapchain");
		return result;
	}

	// Setup depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// Setup depth buffer view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Error creating texture for stencil");
		return result;
	}

	result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (FAILED(result))
	{
		throw std::runtime_error("Error creating view for stencil");
		return result;
	}

	// Create render target and render target view
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
	if (FAILED(result))
	{
		throw std::runtime_error("Error getting back buffer");
		return result;
	}

	result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
	if (FAILED(result))
	{
		throw std::runtime_error("Error creating render target view");
		return result;
	}

	// Bind RTV and DSV to output merger stage
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_Width);
	viewport.Height = static_cast<float>(m_Height);
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.0f;

	m_pDeviceContext->RSSetViewports(1, &viewport);

	m_IsInitialized = true;

	// Clean up factory after use
	pDxgiFactory->Release();

	return result;
}

void DirectX_Renderer::CreateMeshes(std::vector<MeshData*>& pMeshes)
{
	VehicleEffect* effect = new VehicleEffect(m_pDevice, std::wstring{ L"Resources/PosCol3D.fx" });
	m_pMeshes.push_back(new DirectXMesh(m_pDevice, effect, pMeshes[0]));

	ThrusterEffect* thrusterEffect = new ThrusterEffect(m_pDevice, std::wstring{ L"Resources/Thruster.fx" });
	m_pMeshes.push_back(new DirectXMesh(m_pDevice, thrusterEffect, pMeshes[1]));
}

void DirectX_Renderer::CreateTextureSurfaces(std::vector<MeshData*>& pMeshes)
{
	for (auto* mesh : pMeshes)
	{
		for (auto* texture : mesh->textures)
		{
			if (texture == nullptr)
			{
				continue;
			}

			auto surface = texture->GetSurface();

			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			D3D11_TEXTURE2D_DESC desc{};
			desc.Width = surface->w;
			desc.Height = surface->h;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = format;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA initData{};
			initData.pSysMem = surface->pixels;
			initData.SysMemPitch = static_cast<UINT>(surface->pitch);
			initData.SysMemSlicePitch = static_cast<UINT>(surface->h * surface->pitch);

			auto* resource = texture->GetResource();
			HRESULT hr = m_pDevice->CreateTexture2D(&desc, &initData, &resource);
			if (FAILED(hr))
			{
				// Clean up texture
				// delete texture;
				throw std::runtime_error("Failed to create texture for direct3d");
			}
			texture->SetResource(resource);

			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
			SRVDesc.Format = format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;

			auto* resourceView = texture->GetSRV();
			hr = m_pDevice->CreateShaderResourceView(resource, &SRVDesc, &resourceView);
			if (FAILED(hr))
			{
				// Clean up texture
				//delete newTexture;
				throw std::runtime_error("Failed to create resource view for texture");
			}
			texture->SetResourceView(resourceView);
		}
	}
}

void DirectX_Renderer::BindTextures()
{
	for (auto* mesh : m_pMeshes)
	{
		auto texturesToLoad = mesh->GetMeshData()->textures;

		// Diffuse
		if (texturesToLoad[0] != nullptr)
		{
			mesh->SetTexture(DirectXMesh::TextureType::Diffuse, texturesToLoad[0]);
		}
		
		// Normal
		if (texturesToLoad[1] != nullptr)
		{
			mesh->SetTexture(DirectXMesh::TextureType::Normal, texturesToLoad[1]);
		}

		// Specular
		if (texturesToLoad[2] != nullptr)
		{
			mesh->SetTexture(DirectXMesh::TextureType::Specular, texturesToLoad[2]);
		}

		// Glossiness
		if (texturesToLoad[3] != nullptr)
		{
			mesh->SetTexture(DirectXMesh::TextureType::Glossiness, texturesToLoad[3]);
		}
	}
}
