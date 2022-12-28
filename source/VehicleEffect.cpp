#include "pch.h"
#include "VehicleEffect.h"

VehicleEffect::VehicleEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: BaseEffect(pDevice, assetFile)
{
	CreateLayout(pDevice);

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
}


VehicleEffect::~VehicleEffect()
{
	m_pInputLayout->Release();
}


void VehicleEffect::SetNormalMap(Texture* pNormalMap)
{
	if (m_pNormalMapVariable)
	{
		m_pNormalMapVariable->SetResource(pNormalMap->GetSRV());
	}
}

void VehicleEffect::SetSpecularMap(Texture* pSpecularMap)
{
	if (m_pSpecularMapVariable)
	{
		m_pSpecularMapVariable->SetResource(pSpecularMap->GetSRV());
	}
}

void VehicleEffect::SetGlossinessMap(Texture* pGlossinessMap)
{
	if (m_pGlossinessMapVariable)
	{
		m_pGlossinessMapVariable->SetResource(pGlossinessMap->GetSRV());
	}
}

void VehicleEffect::CreateLayout(ID3D11Device* pDevice)
{
	// Vertex layout
	const uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	// Position
	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// UV coordinates
	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Normal coordinates
	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Tangent coordinates
	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	

	// Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffectTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout
	);

	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create input layout");
		return;
	}
}
