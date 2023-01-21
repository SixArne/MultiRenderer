#include "pch.h"
#include "CPU_Renderer.h"
#include "SDL.h"
#include "SDL_surface.h"
#include "DataTypes.h"
#include <string>
#include "Texture.h"
#include "Camera.h"
#include "Shading.h"
#include "Mesh.h"
#include <ppl.h>
#include "RenderConfig.h"
#include "Utils.h"

inline float EdgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
{
	// point to side => c - a
	// side to end => b - a
	return Vector2::Cross(b - a, c - a);
}

CPU_Renderer::CPU_Renderer(SDL_Window* pWindow, Camera* pCamera, std::vector<MeshData*> pMeshes)
	: BaseRenderer(pWindow, pCamera)
{
	m_RendererColor = ColorRGB{ 0.39f * 255.f, 0.39f * 255.f, 0.39f * 255 };
	m_UniformColor = ColorRGB{ 0.1f * 255.f, 0.1f * 255.f, 0.1f * 255 };
	m_CurrentColor = m_RendererColor;

	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);

	// Texture maps

	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	m_pDepthBufferPixels = new float[m_Width * m_Height];

	CreateMeshes(pMeshes);
}


void CPU_Renderer::CreateMeshes(std::vector<MeshData*>& pMeshes)
{
	std::for_each(pMeshes.begin(), pMeshes.end(), [this](MeshData* mesh) 
		{
			CPU_Mesh* newMesh = new CPU_Mesh(mesh, PrimitiveTopology::TriangleList);
			m_pMeshes.push_back(newMesh);
		}
	);
}

CPU_Renderer::~CPU_Renderer()
{
	for (auto* mesh : m_pMeshes)
	{
		delete mesh;
	}

	delete[] m_pDepthBufferPixels;
}

void CPU_Renderer::Update(Timer* pTimer)
{
	if (RENDER_CONFIG->ShouldRotate())
	{
		for (CPU_Mesh* CPUMesh : m_pMeshes)
		{
			MeshData* mesh = CPUMesh->GetMeshData();

			// 1 deg per second
			const float degreesPerSecond = RENDER_CONFIG->GetRotationSpeed();
			mesh->AddRotationY((degreesPerSecond * pTimer->GetElapsed()) * TO_RADIANS);
		}
	}
}

void CPU_Renderer::Render()
{
	// call to base render function for uniform color changes
	BaseRenderer::Render();

	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	RenderFrame();



	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void CPU_Renderer::RenderFrame()
{
	// Transform from World -> View -> Projected -> Raster
	VertexTransformationFunction();

	// Make float array size of image that will act as depth buffer
	std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);

	// Clear back buffer

	SDL_FillRect(m_pBackBuffer, &m_pBackBuffer->clip_rect, SDL_MapRGB(m_pBackBuffer->format, m_CurrentColor.r, m_CurrentColor.g, m_CurrentColor.b));

	// Update current rendering mesh
	auto mesh = m_pMeshes[0];
	m_pCurrentMeshData = mesh->GetMeshData();

	if (mesh->GetPrimitiveTopology() == PrimitiveTopology::TriangleList)
	{
		const uint32_t amountOfTriangles = ((uint32_t)m_pCurrentMeshData->indices.size()) / 3;

		concurrency::parallel_for(0u, amountOfTriangles, [=, this](int index)
			{
				const Vertex_Out vertex1 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[3 * index]];
				const Vertex_Out vertex2 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[3 * index + 1]];
				const Vertex_Out vertex3 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[3 * index + 2]];

				// cull triangles
				if (vertex1.position.x < 0 || vertex2.position.x < 0 || vertex3.position.x < 0
					|| vertex1.position.x > m_Width || vertex2.position.x > m_Width || vertex3.position.x > m_Width
					|| vertex1.position.y < 0 || vertex2.position.y < 0 || vertex3.position.y < 0
					|| vertex1.position.y > m_Height || vertex2.position.y > m_Height || vertex3.position.y > m_Height
					)
				{
					return;
				}

				RenderTriangle(vertex1, vertex2, vertex3);
			}
		);
	}
	else if (mesh->GetPrimitiveTopology() == PrimitiveTopology::TriangleStrip)
	{
		for (uint32_t indice{}; indice < m_pCurrentMeshData->indices.size() - 2; indice++)
		{
			const Vertex_Out vertex1 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[indice]];
			Vertex_Out vertex2{};
			Vertex_Out vertex3{};

			if (indice & 1)
			{
				vertex2 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[indice + 2]];
				vertex3 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[indice + 1]];
			}
			else
			{
				vertex2 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[indice + 1]];
				vertex3 = mesh->GetVerticesOut()[m_pCurrentMeshData->indices[indice + 2]];
			}

			if (vertex1.position.x < 0 || vertex2.position.x < 0 || vertex3.position.x < 0
				|| vertex1.position.x > m_Width || vertex2.position.x > m_Width || vertex3.position.x > m_Width
				|| vertex1.position.y < 0 || vertex2.position.y < 0 || vertex3.position.y < 0
				|| vertex1.position.y > m_Height || vertex2.position.y > m_Height || vertex3.position.y > m_Height
				)
			{
				return;
			}

			RenderTriangle(vertex1, vertex2, vertex3);
		}
	}
}

void CPU_Renderer::VertexTransformationFunction() const
{
	// Calculate once
	MeshData* mesh = m_pMeshes[0]->GetMeshData();

	Matrix worldMatrix = mesh->scaleMatrix * mesh->rotationMatrix * mesh->transformMatrix;
	const auto worldViewProjectionMatrix = worldMatrix * m_pCamera->viewMatrix * m_pCamera->projectionMatrix;

	m_pMeshes[0]->GetVerticesOut().clear();

	// Loop over indices (every 3 indices is triangle)
	for (const auto& vertex : mesh->vertices)
	{
		Vertex_Out rasterVertex{};


		auto position = Vector4{ vertex.position, 1 };

		// Transform model to raster (screen space)
		auto transformedVertex = worldViewProjectionMatrix.TransformPoint(position);
		rasterVertex.viewDirection = worldMatrix.TransformPoint(vertex.position) - m_pCamera->origin;

		// perspective divide
		transformedVertex.x /= transformedVertex.w;
		transformedVertex.y /= transformedVertex.w;
		transformedVertex.z /= transformedVertex.w;

		// NDC to raster coordinates
		transformedVertex.x = ((transformedVertex.x + 1) * (float)m_Width) / 2.f;
		transformedVertex.y = ((1 - transformedVertex.y) * (float)m_Height) / 2.f;

		// Add vertex to vertices out and color

		rasterVertex.position = transformedVertex;
		rasterVertex.uv = vertex.uv;

		Vector3 transformedNormals = worldMatrix.TransformVector(vertex.normal);
		transformedNormals.Normalize();

		Vector3 transformedTangent = worldMatrix.TransformVector(vertex.tangent);
		transformedTangent.Normalize();

		rasterVertex.normal = transformedNormals;
		rasterVertex.tangent = transformedTangent;

		m_pMeshes[0]->GetVerticesOut().push_back(rasterVertex);
	}
}

void CPU_Renderer::RenderTriangle(Vertex_Out vertex1, Vertex_Out vertex2, Vertex_Out vertex3)
{
	const Vector2 v0 = Vector2{ vertex1.position.x, vertex1.position.y };
	const Vector2 v1 = Vector2{ vertex2.position.x, vertex2.position.y };
	const Vector2 v2 = Vector2{ vertex3.position.x, vertex3.position.y };

	// create and clamp bounding box top left
	int maxX{}, maxY{};
	maxX = static_cast<int>(std::max(v0.x, std::max(v1.x, v2.x)));
	maxY = static_cast<int>(std::max(v0.y, std::max(v1.y, v2.y)));

	int minX{}, minY{};
	minX = static_cast<int>(std::min(v0.x, std::min(v1.x, v2.x)));
	minY = static_cast<int>(std::min(v0.y, std::min(v1.y, v2.y)));

	minX = std::clamp(minX, 0, m_Width);
	minY = std::clamp(minY, 0, m_Height);

	maxX = std::clamp(maxX, 0, m_Width);
	maxY = std::clamp(maxY, 0, m_Height);

	for (int px{ minX }; px <= maxX; ++px)
	{
		for (int py{ minY }; py <= maxY; ++py)
		{
			if (RENDER_CONFIG->ShouldRenderBoundingBox())
			{
				ColorRGB finalColor{ 1,1,1 };

				//Update Color in Buffer
				finalColor.MaxToOne();

				m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(finalColor.r * 255),
					static_cast<uint8_t>(finalColor.g * 255),
					static_cast<uint8_t>(finalColor.b * 255));

				continue;
			}

			Vector2 point{ (float)px, (float)py };

			// Barycentric coordinates
			float w0 = EdgeFunction(v1, v2, point);
			float w1 = EdgeFunction(v2, v0, point);
			float w2 = EdgeFunction(v0, v1, point);

			// In triangle
			bool isInTriangle{};

			// culling
			switch (RENDER_CONFIG->GetCurrentCullMode())
			{
				case RenderConfig::CULL_MODE::BACK:
					{
						isInTriangle = w0 >= 0 && w1 >= 0 && w2 >= 0;
					}
					break;
				case RenderConfig::CULL_MODE::FRONT:
					{
						isInTriangle = w0 < 0 && w1 < 0 && w2 < 0;
					}
					break;
				case RenderConfig::CULL_MODE::NONE:
					{
						isInTriangle = (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
					}
					break;
			}

			if (isInTriangle)
			{
				const float area = w0 + w1 + w2;
				w0 /= area;
				w1 /= area;
				w2 /= area;


				// Get the hit point Z with the barycentric weights
				float z = 1.f / ((w0 / vertex1.position.z) + (w1 / vertex2.position.z) + (w2 / vertex3.position.z));

				if (z < 0 || z > 1)
				{
					continue;
				}

				const int pixelZIndex = py * m_Width + px;

				// If new z value of pixel is lower than stored:
				if (z < m_pDepthBufferPixels[pixelZIndex])
				{
					m_pDepthBufferPixels[pixelZIndex] = z;
					const float wInterpolated = 1.f / ((w0 / vertex1.position.w) + (w1 / vertex2.position.w) + (w2 / vertex3.position.w));

					// uv interpolated
					Vector2 uvInterpolated = (vertex1.uv * (w0 / vertex1.position.w)) + (vertex2.uv * (w1 / vertex2.position.w)) + (vertex3.uv * (w2 / vertex3.position.w));
					uvInterpolated *= wInterpolated;

					uvInterpolated.x = std::clamp(uvInterpolated.x, 0.f, 1.f);
					uvInterpolated.y = std::clamp(uvInterpolated.y, 0.f, 1.f);

					// normal interpolated
					Vector3 normalInterpolated =
						(vertex1.normal * (w0 / vertex1.position.w)) +
						(vertex2.normal * (w1 / vertex2.position.w)) +
						(vertex3.normal * (w2 / vertex3.position.w));
					normalInterpolated *= wInterpolated;
					normalInterpolated.Normalize();

					// tangent interpolated
					Vector3 tangentInterpolated = (vertex1.tangent * (w0 / vertex1.position.w)) + (vertex2.tangent * (w1 / vertex2.position.w)) + (vertex3.tangent * (w2 / vertex3.position.w));
					tangentInterpolated *= wInterpolated;
					tangentInterpolated.Normalize();

					// view dir interpolated
					Vector3 viewDirInterpolated = (vertex1.viewDirection * (w0 / vertex1.position.w)) + (vertex2.viewDirection * (w1 / vertex2.position.w)) + (vertex3.viewDirection * (w2 / vertex3.position.w));
					viewDirInterpolated *= wInterpolated;
					viewDirInterpolated.Normalize();

					Vertex_Out fragmentToShade{};
					fragmentToShade.position = Vector4{ (float)px,(float)py,z, wInterpolated };
					fragmentToShade.uv = uvInterpolated;
					fragmentToShade.normal = normalInterpolated;
					fragmentToShade.tangent = tangentInterpolated;
					fragmentToShade.viewDirection = viewDirInterpolated;


					ColorRGB finalColor{  };

					
					if (!RENDER_CONFIG->ShouldRenderDepthBuffer() && !RENDER_CONFIG->ShouldRenderBoundingBox())
					{
						finalColor = ShadePixel(fragmentToShade);
					}
					else if (RENDER_CONFIG->ShouldRenderDepthBuffer())
					{
						float depthValue = Utils::Remap(z, 0.995f, 1.f);
						finalColor = { depthValue, depthValue, depthValue };
					}

					//Update Color in Buffer
					finalColor.MaxToOne();

					m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
						static_cast<uint8_t>(finalColor.r * 255),
						static_cast<uint8_t>(finalColor.g * 255),
						static_cast<uint8_t>(finalColor.b * 255));
				}
			}
		}
	}
}

ColorRGB CPU_Renderer::ShadePixel(const Vertex_Out& vertex)
{
	// Normal map stuff
	const Vector3 binormal = Vector3::Cross(vertex.normal, vertex.tangent).Normalized();
	const Matrix tangentSpaceAxis = Matrix{ vertex.tangent, binormal, vertex.normal, {0,0,0} };

	// Sample normal
	const ColorRGB normalColor = m_pCurrentMeshData->textures[1]->Sample(vertex.uv);
	

	Vector3 normalSample = { normalColor.r, normalColor.g, normalColor.b };
	normalSample = 2.f * normalSample - Vector3{ 1.f, 1.f, 1.f };
	normalSample = tangentSpaceAxis.TransformPoint(normalSample);
	normalSample.Normalize();

	// Sample color
	const ColorRGB color = m_pCurrentMeshData->textures[0]->Sample(vertex.uv);

	// Sample specular
	const ColorRGB specularColor = m_pCurrentMeshData->textures[2]->Sample(vertex.uv);

	// Sample glossiness
	const ColorRGB glossinessColor = m_pCurrentMeshData->textures[3]->Sample(vertex.uv);

	// Lights
	const Vector3 lightDirection = { .577f, -.577f, .577f };
	const float lightIntensity = 7.f;
	const float shininess = 25.f;
	const ColorRGB ambient = { .025f, .025f, .025f };
	const ColorRGB light = ColorRGB{ 1,1,1 } *lightIntensity;

	float lambertCosine{};
	if (RENDER_CONFIG->ShouldRenderNormalMap())
	{
		lambertCosine = Vector3::Dot(normalSample, -lightDirection);
	}
	else 
	{
		lambertCosine = Vector3::Dot(vertex.normal, -lightDirection);
	}

	if (lambertCosine <= 0)
	{
		return { 0,0,0 };
	}

	

	switch (RENDER_CONFIG->GetCurrentShadingMode())
	{
	case RenderConfig::SHADING_MODE::OBSERVED_AREA:
	{
		return { lambertCosine, lambertCosine, lambertCosine };
	}
	break;
	case RenderConfig::SHADING_MODE::DIFFUSE:
	{
		const ColorRGB diffuse = Shading::Lambert(1.f, color);
		return light * diffuse * lambertCosine;
	}
	break;
	case RenderConfig::SHADING_MODE::SPECULAR:
	{

		const auto specularReflectance = specularColor;
		const auto phongExponent = glossinessColor * shininess;

		const ColorRGB specular = Shading::Phong(
			specularReflectance,
			phongExponent,
			lightDirection,
			vertex.viewDirection,
			vertex.normal
		);

		return specular;
	}

	break;
	case RenderConfig::SHADING_MODE::COMBINED:
	{
		const auto specularReflectance = specularColor;
		const auto phongExponent = glossinessColor * shininess;

		const ColorRGB specular = Shading::Phong(
			specularReflectance,
			phongExponent,
			lightDirection,
			vertex.viewDirection,
			vertex.normal
		);

		const ColorRGB diffuse = Shading::Lambert(1.f, color);

		return ((diffuse * light) + specular + ambient) * lambertCosine;
	}

	break;
	case RenderConfig::SHADING_MODE::ENUM_LENGTH:
		throw std::runtime_error("Unknown mode, bug in code");
	}

	const auto specularReflectance = specularColor;
	const auto phongExponent = glossinessColor * shininess;

	const ColorRGB specular = Shading::Phong(
		specularReflectance,
		phongExponent,
		lightDirection,
		vertex.viewDirection,
		normalSample
	);

	const ColorRGB diffuse = Shading::Lambert(1.f, color);

	return ((diffuse * light) + specular + ambient) * lambertCosine;
}

