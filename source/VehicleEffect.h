#pragma once
#include "Texture.h"
#include "BaseEffect.h"

class VehicleEffect : public BaseEffect
{
public:
	VehicleEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~VehicleEffect();

	virtual void SetNormalMap(Texture* pNormalMap) override;
	virtual void SetSpecularMap(Texture* pSpecularMap) override;
	virtual void SetGlossinessMap(Texture* pGlossinessMap) override;
	
protected:
	virtual void CreateLayout(ID3D11Device* pDevice) override;

private:
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};
};


