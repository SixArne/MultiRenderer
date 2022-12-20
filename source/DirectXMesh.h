#pragma once
#include "Mesh.h"

class ID3D11Device;
class Effect;
class ID3DX11EffectTechnique;
class ID3D11DeviceContext;
class Matrix;
class Vector3;
class ID3D11Buffer;
class ID3D11InputLayout;

class DirectXMesh
{
public:
	enum class TextureType
	{
		Normal,
		Diffuse,
		Glossiness,
		Specular
	};

	DirectXMesh(ID3D11Device* pDevice, MeshData* meshData);
	~DirectXMesh();
	DirectXMesh(const DirectXMesh&) = delete;
	DirectXMesh(DirectXMesh&&) noexcept = delete;
	DirectXMesh& operator=(const DirectXMesh&) = delete;
	DirectXMesh& operator=(DirectXMesh&&) noexcept = delete;

	MeshData* GetMeshData() { return m_pMeshData; };
	void Render(ID3D11DeviceContext* pDeviceContext, Matrix& wvp) const;
	void SetTexture(TextureType type, Texture* texture);
	void SetLightDirection(Vector3& lightDir);
	void SetWorldMatrix(Matrix& world);
	void SetViewInverse(Matrix& viewInverse);

private:
	MeshData* m_pMeshData{};

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
	ID3D11InputLayout* m_pInputLayout{};

	uint32_t m_NumIndices{};
};

