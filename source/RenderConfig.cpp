#include "pch.h"
#include "RenderConfig.h"
#include <iostream>

RenderConfig* RenderConfig::s_RenderInstance{ nullptr };

RenderConfig* RenderConfig::GetInstance()
{
	if (s_RenderInstance == nullptr)
	{
		s_RenderInstance = new RenderConfig;
	}

	return s_RenderInstance;
}

void RenderConfig::CycleRenderer()
{
	// stop vulkan renderer when switching
	if (m_ShouldUseVulkan)
	{
		m_ShouldUseVulkan = false;
	}

	const auto APICycleIndex = static_cast<int8_t>(m_CurrentAPI);
	const auto newAPICycleIndex = (APICycleIndex + 1) % static_cast<int8_t>(API::ENUM_LENGTH);

	m_CurrentAPI = static_cast<API>(newAPICycleIndex);

	std::cout << "\033[33m"; // TEXT COLOR
	switch (m_CurrentAPI)
	{
	case API::DIRECTX:
		std::cout << "API: DirectX" << "\n";
		break;
	case API::CPU:
		std::cout << "API: CPU" << "\n";
		break;
	case API::ENUM_LENGTH:
		throw std::runtime_error("Unknown API, bug in code");
	}
	std::cout << "\033[38m"; // TEXT COLOR
}

void RenderConfig::ToggleRotation()
{
	m_ShouldRotate = !m_ShouldRotate;

	if (m_ShouldRotate)
	{
		std::cout << "\033[33m"; // TEXT COLOR
		std::cout << "[ENABLE] Rotation" << std::endl;
		std::cout << "\033[37m"; // TEXT COLOR WHITE
	}
	else 
	{
		std::cout << "\033[33m"; // TEXT COLOR
		std::cout << "[DISABLE] Rotation" << std::endl;
		std::cout << "\033[37m"; // TEXT COLOR WHITE
	}
}

void RenderConfig::ToggleThruster()
{
	m_ShouldRenderThruster = !m_ShouldRenderThruster;

	if (m_ShouldRenderThruster)
	{
		std::cout << "\033[32m"; // TEXT COLOR
		std::cout << "[ENABLE] Thruster" << std::endl;
		std::cout << "\033[37m"; // TEXT COLOR WHITE
	}
	else
	{
		std::cout << "\033[32m"; // TEXT COLOR
		std::cout << "[DISABLE] Thurster" << std::endl;
		std::cout << "\033[37m"; // TEXT COLOR WHITE
	}
}

void RenderConfig::CycleSamplingStates()
{
	const auto samplingCycleIndex = static_cast<int8_t>(m_CurrentSampleState);
	const auto newSamplingCycleIndex = (samplingCycleIndex + 1) % static_cast<int8_t>(SAMPLE_MODE::ENUM_LENGTH);

	m_CurrentSampleState = static_cast<SAMPLE_MODE>(newSamplingCycleIndex);

	std::cout << "\033[32m"; // TEXT COLOR

	switch (m_CurrentSampleState)
	{
	case SAMPLE_MODE::POINT:
		std::cout << "Point filtering" << "\n";
		break;
	case SAMPLE_MODE::LINEAR:
		std::cout << "Linear filtering" << "\n";
		break;
	case SAMPLE_MODE::ANISOTROPIC:
		std::cout << "Anisotropic filtering" << "\n";
		break;
	case SAMPLE_MODE::ENUM_LENGTH:
		throw std::runtime_error("Unknown API, bug in code");
	}

	std::cout << "\033[37m"; // TEXT COLOR WHITE
}

bool RenderConfig::ShouldRenderThruster()
{
	return m_ShouldRenderThruster;

	if (m_ShouldRenderThruster)
	{
		std::cout << "Started Rendering thruster" << std::endl;
	}
	else
	{
		std::cout << "Stopped rendering thruster" << std::endl;
	}
}

RenderConfig::SAMPLE_MODE RenderConfig::GetCurrentSampleState()
{
	return m_CurrentSampleState;
}

void RenderConfig::CycleShadingMode()
{
	const auto shadingCycleIndex = static_cast<int8_t>(m_CurrentShadingMode);
	const auto newShadingCycleIndex = (shadingCycleIndex + 1) % static_cast<int8_t>(SHADING_MODE::ENUM_LENGTH);

	m_CurrentShadingMode = static_cast<SHADING_MODE>(newShadingCycleIndex);

	std::cout << "\033[35m"; // TEXT COLOR

	switch (m_CurrentShadingMode)
	{
	case SHADING_MODE::COMBINED:
		std::cout << "Shading mode: Combined" << "\n";
		break;
	case SHADING_MODE::DIFFUSE:
		std::cout << "Shading mode: Diffuse" << "\n";
		break;
	case SHADING_MODE::OBSERVED_AREA:
		std::cout << "Shading mode: Observed area" << "\n";
		break;
	case SHADING_MODE::SPECULAR:
		std::cout << "Shading mode: Specular" << "\n";
		break;
	case SHADING_MODE::ENUM_LENGTH:
		throw std::runtime_error("Unknown API, bug in code");
	}

	std::cout << "\033[38m"; // TEXT COLOR
}

void RenderConfig::ToggleNormapMap()
{
	m_ShouldRenderNormalMap = !m_ShouldRenderNormalMap;

	if (m_ShouldRenderNormalMap)
	{
		std::cout << "\033[35m"; // TEXT COLOR
		std::cout << "Rendering normal map! [Don't forget to disable this later]" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
	else
	{
		std::cout << "\033[35m"; // TEXT COLOR
		std::cout << "Stopped rendering normal map!" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
}

void RenderConfig::ToggleDepthBuffer()
{
	m_ShouldRenderDepthBuffer = !m_ShouldRenderDepthBuffer;

	if (m_ShouldRenderDepthBuffer)
	{
		std::cout << "\033[35m"; // TEXT COLOR
		std::cout << "Showing depth visualization [Don't forget to disable this later]" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
	else
	{
		std::cout << "\033[35m"; // TEXT COLOR
		std::cout << "Stopped showing depth visualization" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
}

void RenderConfig::ToggleBoundingBox()
{
	m_ShouldRenderBoundingBox = !m_ShouldRenderBoundingBox;

	if (m_ShouldRenderBoundingBox)
	{
		std::cout << "\033[35m"; // TEXT COLOR
		std::cout << "Showing bounding box optimizations [Don't forget to disable this later]" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
	else
	{
		std::cout << "\033[35m"; // TEXT COLOR
		std::cout << "Stopped showing bounding box optimizations" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
}

bool RenderConfig::ShouldRenderNormalMap()
{
	return m_ShouldRenderNormalMap;
}

bool RenderConfig::ShouldRenderDepthBuffer()
{
	return m_ShouldRenderDepthBuffer;
}

bool RenderConfig::ShouldRenderBoundingBox()
{
	return m_ShouldRenderBoundingBox;
}

RenderConfig::SHADING_MODE RenderConfig::GetCurrentShadingMode()
{
	return m_CurrentShadingMode;
}

void RenderConfig::ToggleVulkan()
{
	m_ShouldUseVulkan = !m_ShouldUseVulkan;

	if (m_ShouldUseVulkan)
	{
		std::cout << "\033[31m"; // TEXT COLOR RED
		std::cout << "[ENABLE] Vulkan" << std::endl;
		std::cout << "\033[37m"; // TEXT COLOR WHITE
	}
	else
	{
		std::cout << "\033[31m"; // TEXT COLOR RED
		std::cout << "[DISABLE] Vulkan" << std::endl;
		std::cout << "\033[37m"; // TEXT COLOR WHITE
	}
}

bool RenderConfig::GetShouldUseVulkan()
{
	return m_ShouldUseVulkan;
}

void RenderConfig::DestroyInstance()
{
	delete s_RenderInstance;
	s_RenderInstance = nullptr;
}

bool RenderConfig::ShouldRotate()
{
	return m_ShouldRotate;
}

bool RenderConfig::ShouldUseUniformColor()
{
	return m_ShouldUseUniformColor;
}

void RenderConfig::ToggleUniformColor()
{
	m_ShouldUseUniformColor = !m_ShouldUseUniformColor;

	if (m_ShouldUseUniformColor)
	{
		std::cout << "\033[33m"; // TEXT COLOR
		std::cout << "Displaying uniform rendering color" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
	else
	{
		std::cout << "\033[33m"; // TEXT COLOR
		std::cout << "Showing individual rendering color" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
}

void RenderConfig::CycleCullMode()
{
	const auto cullCycleIndex = static_cast<int8_t>(m_CurrentCullMode);
	const auto newCullCycleIndex = (cullCycleIndex + 1) % static_cast<int8_t>(CULL_MODE::ENUM_LENGTH);

	m_CurrentCullMode = static_cast<CULL_MODE>(newCullCycleIndex);

	std::cout << "\033[33m"; // TEXT COLOR

	switch (m_CurrentCullMode)
	{
	case CULL_MODE::BACK:
		std::cout << "Cull mode: BACK" << "\n";
		break;
	case CULL_MODE::FRONT:
		std::cout << "Cull mode: FRONT" << "\n";
		break;
	case CULL_MODE::NONE:
		std::cout << "Cull mode: NONE" << "\n";
		break;
	case CULL_MODE::ENUM_LENGTH:
		throw std::runtime_error("Unknown API, bug in code");
	}

	std::cout << "\033[38m"; // TEXT COLOR
}

bool RenderConfig::ShouldPrintFPS()
{
	return m_ShouldPrintFPS;
}

float RenderConfig::GetRotationSpeed()
{
	return m_RotationSpeed;
}

void RenderConfig::PrintInstructions()
{
	std::cout << "\033[33m"; // TEXT COLOR
	std::cout << "[Key Bindings - SHARED]" << std::endl;
	std::cout << "\t[F1] Toggle Rasterizer Mode (HARDWARE/SOFTWARE)" << std::endl;
	std::cout << "\t[F2] Toggle Vehicle Rotation (ON/OFF)" << std::endl;
	std::cout << "\t[F9] Cycle CullMode (BACK/FRONT/NONE)" << std::endl;
	std::cout << "\t[F10] Toggle Uniform ClearColor (ON/OFF)" << std::endl;
	std::cout << "\t[F11] Toggle Print FPS (ON/OFF)" << std::endl;
	std::cout << std::endl;
	std::cout << "\033[32m"; // TEXT COLOR
	std::cout << "[Key Bindings - HARDWARE]" << std::endl;
	std::cout << "\t[F3] Toggle FireFX (ON / OFF)" << std::endl;
	std::cout << "\t[F4] Cycle Sampler State (POINT / LINEAR / ANISOTROPIC)" << std::endl;
	std::cout << std::endl;
	std::cout << "\033[35m"; // TEXT COLOR
	std::cout << "[Key Bindings - SOFTWARE]" << std::endl;
	std::cout << "\t[F5] Cycle Shading Mode (COMBINED / OBSERVED_AREA / DIFFUSE / SPECULAR)" << std::endl;
	std::cout << "\t[F6] Toggle NormalMap (ON / OFF" << std::endl;
	std::cout << "\t[F7] Toggle DepthBuffer Visualization (ON / OFF)" << std::endl;
	std::cout << "\t[F8] Toggle BoundingBox Visualization (ON / OFF)" << std::endl;
	std::cout << "\033[0m"; // TEXT COLOR
	std::cout << std::endl;
	std::cout << "\033[31m"; // TEXT COLOR
	std::cout << "[Extra Features]" << std::endl;
	std::cout << "\t- Vulkan renderer" << std::endl;
	std::cout << "\t- CPU multi-threaded" << std::endl;
	std::cout << std::endl;
	std::cout << "[Extra Key Bindings - Vulkan]" << std::endl;
	std::cout << "\t[V] Toggle Vulkan (ON / OFF" << std::endl;
	std::cout << "\033[0m"; // TEXT COLOR
	std::cout << std::endl;
	std::cout << std::endl;
}

RenderConfig::API RenderConfig::GetCurrentAPI()
{
	return m_CurrentAPI;
}

RenderConfig::CULL_MODE RenderConfig::GetCurrentCullMode()
{
	return m_CurrentCullMode;
}

void RenderConfig::TogglePrintFPS()
{
	m_ShouldPrintFPS = !m_ShouldPrintFPS;

	if (m_ShouldPrintFPS)
	{
		std::cout << "\033[33m"; // TEXT COLOR
		std::cout << "[Enable] FPS" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
	else
	{
		std::cout << "\033[33m"; // TEXT COLOR
		std::cout << "[Disable] FPS" << std::endl;
		std::cout << "\033[38m"; // TEXT COLOR
	}
}