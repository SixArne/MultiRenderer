#include "pch.h"
#include "D3DTexture.h"

D3DTexture::~D3DTexture()
{
	if (m_pResource != nullptr)
	{
		m_pResource->Release();
	}

	if (m_pResourceView != nullptr)
	{
		m_pResourceView->Release();
	}
}

D3DTexture* D3DTexture::LoadFromFile(ID3D11Device* device, const std::string& path)
{
	return nullptr;
}

ID3D11ShaderResourceView* D3DTexture::GetSRV()
{
	return nullptr;
}
