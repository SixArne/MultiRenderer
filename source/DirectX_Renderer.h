#pragma once
#include "BaseRenderer.h"
// DirectX Headers


#include "Mesh.h"
#include <d3d11.h>

class DirectXMesh;

class DirectX_Renderer: public BaseRenderer
{
public:
	DirectX_Renderer(SDL_Window* pWindow, Camera* pCamera, std::vector<MeshData*> pMeshes);
	virtual ~DirectX_Renderer() override;

	virtual void Update(Timer* pTimer) override;
	virtual void Render() override;

private:
	// DirectX members
	ID3D11Device* m_pDevice{};
	ID3D11DeviceContext* m_pDeviceContext{};
	IDXGISwapChain* m_pSwapChain{};

	// Stencil
	ID3D11Texture2D* m_pDepthStencilBuffer{};
	ID3D11DepthStencilView* m_pDepthStencilView{};

	// Back buffer
	ID3D11Texture2D* m_pRenderTargetBuffer{};
	ID3D11RenderTargetView* m_pRenderTargetView{};

	std::vector<DirectXMesh*> m_pMeshes{};
	bool m_IsInitialized{ false };

	//DIRECTX
	HRESULT InitializeDirectX();
	//...

	void CreateMeshes(std::vector<MeshData*>& pMeshes);
	void CreateTextureSurfaces(std::vector<MeshData*>& pMeshes);
	void BindTextures();
};

