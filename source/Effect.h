#pragma once
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11EffectTechnique* GetTechnique();
	ID3DX11Effect* GetEffect();
	ID3DX11EffectMatrixVariable* GetMatrixVariable();
	void SetDiffuseMap(Texture* pDiffuseMap);
	void SetNormalMap(Texture* pNormalMap);
	void SetSpecularMap(Texture* pSpecularMap);
	void SetGlossinessMap(Texture* pGlossinessMap);
	void SetLightDirection(Vector3& lightDirection);
	void SetWorldMatrix(Matrix& worldMatrix);
	void SetInverseViewMatrix(Matrix& inverseView);
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

private:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};
	ID3DX11EffectVectorVariable* m_pLightDirVariable{};
	ID3DX11EffectMatrixVariable* m_pWorldVariable{};
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable{};
};


