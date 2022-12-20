#pragma once
#include "BaseRenderer.h"
#include "DataTypes.h"
#include "Camera.h"
#include "Timer.h"
#include "CPU_Mesh.h"
#include "Mesh.h"

class SDL_Surface;

class CPU_Renderer : public BaseRenderer
{
public:
	CPU_Renderer(SDL_Window* pWindow, Camera* pCamera, std::vector<MeshData*> pMeshes);
	virtual ~CPU_Renderer() override;

	virtual void Update(Timer* pTimer) override;
	virtual void Render() override;

private:
	SDL_Surface* m_pFrontBuffer{ nullptr };
	SDL_Surface* m_pBackBuffer{ nullptr };
	uint32_t* m_pBackBufferPixels{};

	float* m_pDepthBufferPixels{};

	std::vector<CPU_Mesh*> m_pMeshes{};
	MeshData* m_pCurrentMeshData{};

	void RenderFrame();
	void VertexTransformationFunction() const; //W2 version
	void RenderTriangle(Vertex_Out v1, Vertex_Out v2, Vertex_Out v3);
	ColorRGB ShadePixel(const Vertex_Out& vertex);


	// Creation functions
	void CreateMeshes(std::vector<MeshData*>& pMeshes);
};




