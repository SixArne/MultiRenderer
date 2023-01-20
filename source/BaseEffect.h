#include "Texture.h"

#pragma once
class BaseEffect
{
public:
	BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~BaseEffect();

	ID3DX11EffectTechnique* GetTechnique();
	ID3DX11Effect* GetEffect();
	ID3DX11EffectMatrixVariable* GetMatrixVariable();
	void SetDiffuseMap(Texture* pDiffuseMap);
	void SetLightDirection(Vector3& lightDirection);
	void SetWorldMatrix(Matrix& worldMatrix);
	void SetInverseViewMatrix(Matrix& inverseView);
	virtual void SetNormalMap(Texture* pNormalMap) {};
	virtual void SetSpecularMap(Texture* pSpecularMap) {};
	virtual void SetGlossinessMap(Texture* pGlossinessMap) {};
	ID3D11InputLayout* GetInputLayout();

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

protected:
	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pEffectTechnique{};
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	ID3DX11EffectMatrixVariable* m_pWorldVariable{};
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable{};
	ID3DX11EffectVectorVariable* m_pLightDirVariable{};
	ID3D11InputLayout* m_pInputLayout{};

	ID3D11SamplerState* m_pPointSampler{};
	ID3D11SamplerState* m_pLinearSampler{};
	ID3D11SamplerState* m_pAnisothropicSampler{};

	ID3D11RasterizerState* m_pRasterizerStateBackFace{};
	ID3D11RasterizerState* m_pRasterizerStateFrontFace{};
	ID3D11RasterizerState* m_pRasterizerStateNone{};

	ID3DX11EffectSamplerVariable* m_pSamplerState{};
	ID3DX11EffectRasterizerVariable* m_pRasterizerState{};

	virtual void CreateLayout(ID3D11Device* pDevice) = 0;

public:
	void UpdateSamplerState();
	void UpdateCullModes();

private:
	void SetupSamplers(ID3D11Device* pDevice);
	void SetupCullModes(ID3D11Device* pDevice);
};

