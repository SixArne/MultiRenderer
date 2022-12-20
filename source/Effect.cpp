#include "pch.h"
#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = Effect::LoadEffect(pDevice, assetFile);
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

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::wcout << L"m_pNormalMapVariable is not valid\n";
	}

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::wcout << L"m_pSpecularMapVariable is not valid\n";
	}

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
	{
		std::wcout << L"m_pGlossinessMapVariable is not valid\n";
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
}

Effect::~Effect()
{
	m_pEffect->Release();
}

ID3DX11EffectTechnique* Effect::GetTechnique()
{
	return m_pEffectTechnique;
}

ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectMatrixVariable* Effect::GetMatrixVariable()
{
	return m_pMatWorldViewProjVariable;
}

void Effect::SetDiffuseMap(Texture* pDiffuseMap)
{
	if (m_pDiffuseMapVariable)
	{
		m_pDiffuseMapVariable->SetResource(pDiffuseMap->GetSRV());
	}
}

void Effect::SetNormalMap(Texture* pNormalMap)
{
	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->SetResource(pNormalMap->GetSRV());
	}
}

void Effect::SetSpecularMap(Texture* pSpecularMap)
{
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->SetResource(pSpecularMap->GetSRV());
	}
}

void Effect::SetGlossinessMap(Texture* pGlossinessMap)
{
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->SetResource(pGlossinessMap->GetSRV());
	}
}

void Effect::SetLightDirection(Vector3& lightDirection)
{
	if (m_pLightDirVariable)
	{
		m_pLightDirVariable->SetFloatVector((float*)&lightDirection);
	}
}

void Effect::SetWorldMatrix(Matrix& worldMatrix)
{
	if (m_pWorldVariable)
	{
		m_pWorldVariable->SetMatrix((float*)&worldMatrix);
	}
}

void Effect::SetInverseViewMatrix(Matrix& inverseView)
{
	if (m_pViewInverseVariable)
	{
		m_pViewInverseVariable->SetMatrix((float*)&inverseView);
	}
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
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
