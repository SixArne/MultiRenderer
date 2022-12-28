#include "pch.h"
#include "DirectXMesh.h"
#include "VehicleEffect.h"

DirectXMesh::DirectXMesh(ID3D11Device* pDevice, BaseEffect* effect, MeshData* meshData)
{
	m_pEffect = effect;
	m_pEffectTechnique = m_pEffect->GetTechnique();

	// Create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * static_cast<uint32_t>(meshData->vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = meshData->vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create buffer for mesh");
		return;
	}

	// Create index buffer
	m_NumIndices = static_cast<uint32_t>(meshData->indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	// Re-use
	initData.pSysMem = meshData->indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		throw std::runtime_error("Failed to create input layout");
		return;
	}

	m_pMeshData = meshData;
}

DirectXMesh::~DirectXMesh()
{
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
	
	delete m_pEffect;
}

void DirectXMesh::Render(ID3D11DeviceContext* pDeviceContext, Matrix& wvp) const
{
	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set input layout
	pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

	// Set vertex buffer
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	ID3D11Buffer* pVertexBuffer = m_pVertexBuffer;

	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	m_pEffect->GetMatrixVariable()->SetMatrix((float*)(&wvp));

	// Set indexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p{}; p < techDesc.Passes; p++)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void DirectXMesh::SetTexture(TextureType type, Texture* texture)
{
	switch (type)
	{
	case DirectXMesh::TextureType::Normal:
		m_pEffect->SetNormalMap(texture);
		break;
	case DirectXMesh::TextureType::Diffuse:
		m_pEffect->SetDiffuseMap(texture);
		break;
	case DirectXMesh::TextureType::Glossiness:
		m_pEffect->SetGlossinessMap(texture);
		break;
	case DirectXMesh::TextureType::Specular:
		m_pEffect->SetSpecularMap(texture);
		break;
	default:
		break;
	}
}

void DirectXMesh::SetLightDirection(Vector3& lightDir)
{
	m_pEffect->SetLightDirection(lightDir);
}

void DirectXMesh::SetWorldMatrix(Matrix& world)
{
	m_pEffect->SetWorldMatrix(world);
}

void DirectXMesh::SetViewInverse(Matrix& viewInverse)
{
	m_pEffect->SetInverseViewMatrix(viewInverse);
}
