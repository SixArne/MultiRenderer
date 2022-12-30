#define RENDER_CONFIG (RenderConfig::GetInstance())



#pragma once
class RenderConfig
{
public:
	enum class API
	{
		DIRECTX,
		CPU,
		ENUM_LENGTH
	};

	enum class CULL_MODE
	{
		BACK,
		FRONT,
		NONE,
		ENUM_LENGTH
	};

	enum class SAMPLE_MODE
	{
		POINT,
		LINEAR,
		ANISOTROPIC,
		ENUM_LENGTH
	};

	enum class SHADING_MODE
	{
		COMBINED,
		OBSERVED_AREA,
		DIFFUSE,
		SPECULAR,
		ENUM_LENGTH
	};

	// Singleton getter
	static RenderConfig* GetInstance();

	void CycleRenderer();
	void CycleCullMode();
	
	void ToggleUniformColor();
	void ToggleRotation();
	void TogglePrintFPS();

	bool ShouldRotate();
	bool ShouldUseUniformColor();
	bool ShouldPrintFPS();

	float GetRotationSpeed();
	void PrintInstructions();

	API GetCurrentAPI();
	CULL_MODE GetCurrentCullMode();

	/************************************************************************/
	/* Hardware only                                                        */
	/************************************************************************/
	void ToggleThruster();
	void CycleSamplingStates();
	bool ShouldRenderThruster();
	SAMPLE_MODE GetCurrentSampleState();

	/************************************************************************/
	/* Software only														*/
	/************************************************************************/
	void CycleShadingMode();
	void ToggleNormapMap();
	void ToggleDepthBuffer();
	void ToggleBoundingBox();


	/************************************************************************/
	/* Vulkan																*/
	/************************************************************************/
	void ToggleVulkan();
	bool GetShouldUseVulkan();

	static void DestroyInstance();

private:
	/************************************************************************/
	/* Singleton constructor												*/
	/************************************************************************/
	RenderConfig() = default;

	static RenderConfig* s_RenderInstance;

	bool m_ShouldRotate{ true };
	bool m_ShouldUseUniformColor{ false };
	bool m_ShouldPrintFPS{ false };

	float m_RotationSpeed{ 25.f };
	
	/************************************************************************/
	/* Hardware only														*/
	/************************************************************************/
	bool m_ShouldRenderThruster{ true };
	SAMPLE_MODE m_CurrentSampleState{ SAMPLE_MODE::POINT };


	/************************************************************************/
	/* Software only														*/
	/************************************************************************/
	bool m_ShouldRenderNormalMap{true};
	bool m_ShouldRenderDepthBuffer{ false };
	bool m_ShouldRenderBoundingBox{ false };
	SHADING_MODE m_CurrentShadingMode{ SHADING_MODE::COMBINED };
	
	/************************************************************************/
	/* Vulkan																*/
	/************************************************************************/
	bool m_ShouldUseVulkan{ false };

	CULL_MODE m_CurrentCullMode{ CULL_MODE::BACK };
	API m_CurrentAPI{ API::CPU};
};

