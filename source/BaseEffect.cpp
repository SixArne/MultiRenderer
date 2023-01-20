#include "pch.h"
#include "BaseEffect.h"
#include "RenderConfig.h"

BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = BaseEffect::LoadEffect(pDevice, assetFile);
	m_pEffectTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pEffectTechnique->IsValid())
	{
		std::wcout << L"Technique not valid" << std::endl;
	}

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable is not valid\n";
	}

	// Textures
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable is not valid\n";
	}

	// Matrices
	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
	{
		std::wcout << L"m_pViewInverseVariable is not valid\n";
	}

	m_pWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	if (!m_pWorldVariable->IsValid())
	{
		std::wcout << L"m_pWorldVariable is not valid\n";
	}

	// light
	m_pLightDirVariable = m_pEffect->GetVariableByName("gLightDir")->AsVector();
	if (!m_pLightDirVariable->IsValid())
	{
		std::wcout << L"m_pLightDirVariable is not valid\n";
	}

	m_pSamplerState = m_pEffect->GetVariableByName("gSamplerState")->AsSampler();
	if (!m_pSamplerState->IsValid())
	{
		std::wcout << L"m_pSamplerState is not valid\n";
	}

	m_pRasterizerState = m_pEffect->GetVariableByName("gRasterizerState")->AsRasterizer();
	if (!m_pRasterizerState->IsValid())
	{
		std::wcout << L"m_pRasterizerState is not valid\n";
	}

	SetupSamplers(pDevice);
	SetupCullModes(pDevice);
}

void BaseEffect::SetupSamplers(ID3D11Device* pDevice)
{
	D3D11_SAMPLER_DESC samplerDesc{};

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxAnisotropy = 2;

	HRESULT hr = pDevice->CreateSamplerState(&samplerDesc, &m_pPointSampler);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create point sampler state");
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pLinearSampler);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create point sampler state");
	}

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pAnisothropicSampler);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create point sampler state");
	}

	m_pSamplerState->SetSampler(0, m_pPointSampler);
}

void BaseEffect::SetupCullModes(ID3D11Device* pDevice)
{
	D3D11_RASTERIZER_DESC rasterizerDesc{};

	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

	HRESULT hr = pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerStateBackFace);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create back cull state");
	}

	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	hr = pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerStateFrontFace);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create front cull state");
	}

	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	hr = pDevice->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerStateNone);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create none cull state");
	}

	// default back face
	m_pRasterizerState->SetRasterizerState(0, m_pRasterizerStateBackFace);
}

BaseEffect::~BaseEffect()
{
	m_pEffect->Release();

	m_pPointSampler->Release();
	m_pLinearSampler->Release();
	m_pAnisothropicSampler->Release();

	m_pRasterizerStateFrontFace->Release();
	m_pRasterizerStateBackFace->Release();
	m_pRasterizerStateNone->Release();
}

ID3DX11EffectTechnique* BaseEffect::GetTechnique()
{
	return m_pEffectTechnique;
}

ID3DX11Effect* BaseEffect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectMatrixVariable* BaseEffect::GetMatrixVariable()
{
	return m_pMatWorldViewProjVariable;
}

void BaseEffect::SetDiffuseMap(Texture* pDiffuseMap)
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseMap->GetSRV());
	}
}

void BaseEffect::SetLightDirection(Vector3& lightDirection)
{
	if (m_pLightDirVariable)
	{
		m_pLightDirVariable->SetFloatVector((float*)&lightDirection);
	}
}

void BaseEffect::SetWorldMatrix(Matrix& worldMatrix)
{
	if (m_pWorldVariable)
	{
		m_pWorldVariable->SetMatrix((float*)&worldMatrix);
	}
}

void BaseEffect::SetInverseViewMatrix(Matrix& inverseView)
{
	if (m_pViewInverseVariable)
	{
		m_pViewInverseVariable->SetMatrix((float*)&inverseView);
	}
}

ID3D11InputLayout* BaseEffect::GetInputLayout()
{
	return m_pInputLayout;
}

ID3DX11Effect* BaseEffect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result{};
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect{};

	DWORD shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss{};
			for (uint32_t i{}; i < pErrorBlob->GetBufferSize(); i++)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;
		}
		else
		{
			std::wstringstream ss{};
			ss << " EffectLoader: Failed to CreateEffectFromFile!\n Path: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}

		throw std::runtime_error("Failed to compile shader");
	}

	return pEffect;
}

void BaseEffect::UpdateSamplerState()
{
	auto samplerState = RENDER_CONFIG->GetCurrentSampleState();

	switch (samplerState)
	{
	case RenderConfig::SAMPLE_MODE::POINT:
		m_pSamplerState->SetSampler(0, m_pPointSampler);
		break;
	case RenderConfig::SAMPLE_MODE::LINEAR:
		m_pSamplerState->SetSampler(0, m_pLinearSampler);
		break;
	case RenderConfig::SAMPLE_MODE::ANISOTROPIC:
		m_pSamplerState->SetSampler(0, m_pAnisothropicSampler);
		break;
	}
}

void BaseEffect::UpdateCullModes()
{
	auto cullState = RENDER_CONFIG->GetCurrentCullMode();

	switch (cullState)
	{
	case RenderConfig::CULL_MODE::FRONT:
		m_pRasterizerState->SetRasterizerState(0, m_pRasterizerStateFrontFace);
		break;
	case RenderConfig::CULL_MODE::BACK:
		m_pRasterizerState->SetRasterizerState(0, m_pRasterizerStateBackFace);
		break;
	case RenderConfig::CULL_MODE::NONE:
		m_pRasterizerState->SetRasterizerState(0, m_pRasterizerStateNone);
		break;
	}
}

