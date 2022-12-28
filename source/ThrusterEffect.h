#pragma once
#include "BaseEffect.h"
class ThrusterEffect :
    public BaseEffect
{
public:
	ThrusterEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~ThrusterEffect();

protected:
	virtual void CreateLayout(ID3D11Device* pDevice) override;
};

