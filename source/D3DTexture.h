#pragma once
class D3DTexture
{
public:
	~D3DTexture();

	static D3DTexture* LoadFromFile(ID3D11Device* device, const std::string& path);
	ID3D11ShaderResourceView* GetSRV();

private:
	D3DTexture() = default;
	ID3D11Texture2D* m_pResource{};
	ID3D11ShaderResourceView* m_pResourceView{};
};

