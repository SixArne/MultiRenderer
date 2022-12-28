#include "pch.h"
#include "RenderConfig.h"

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
	const auto APICycleIndex = static_cast<int8_t>(m_CurrentAPI);
	const auto newAPICycleIndex = (APICycleIndex + 1) % static_cast<int8_t>(API::ENUM_LENGTH);

	m_CurrentAPI = static_cast<API>(newAPICycleIndex);

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
}

void RenderConfig::ToggleRotation()
{
	m_ShouldRotate = !m_ShouldRotate;
}

void RenderConfig::ToggleThruster()
{
	m_ShouldRenderThruster = !m_ShouldRenderThruster;
}

void RenderConfig::CycleSamplingStates()
{
	const auto samplingCycleIndex = static_cast<int8_t>(m_CurrentSampleState);
	const auto newSamplingCycleIndex = (samplingCycleIndex + 1) % static_cast<int8_t>(SAMPLE_MODE::ENUM_LENGTH);

	m_CurrentSampleState = static_cast<SAMPLE_MODE>(newSamplingCycleIndex);

	switch (m_CurrentSampleState)
	{
	case SAMPLE_MODE::POINT:
		std::cout << "API: DirectX" << "\n";
		break;
	case SAMPLE_MODE::LINEAR:
		std::cout << "API: CPU" << "\n";
		break;
	case SAMPLE_MODE::ANISOTROPIC:
		std::cout << "API: CPU" << "\n";
		break;
	case SAMPLE_MODE::ENUM_LENGTH:
		throw std::runtime_error("Unknown API, bug in code");
	}
}

bool RenderConfig::ShouldRenderThruster()
{
	return m_ShouldRenderThruster;
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
}

void RenderConfig::ToggleNormapMap()
{
	m_ShouldRenderNormalMap = !m_ShouldRenderNormalMap;
}

void RenderConfig::ToggleDepthBuffer()
{
	m_ShouldRenderDepthBuffer = !m_ShouldRenderDepthBuffer;
}

void RenderConfig::ToggleBoundingBox()
{
	m_ShouldRenderBoundingBox = !m_ShouldRenderBoundingBox;
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
}

void RenderConfig::CycleCullMode()
{
	const auto cullCycleIndex = static_cast<int8_t>(m_CurrentCullMode);
	const auto newCullCycleIndex = (cullCycleIndex + 1) % static_cast<int8_t>(CULL_MODE::ENUM_LENGTH);

	m_CurrentCullMode = static_cast<CULL_MODE>(newCullCycleIndex);

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
}

bool RenderConfig::ShouldPrintFPS()
{
	return m_ShouldPrintFPS;
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
}