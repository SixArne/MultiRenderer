#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"


struct Vector2;

class Texture
{
public:
	~Texture();

	static Texture* LoadFromFile(const std::string& path);
	SDL_Surface* GetSurface() { return m_pSurface; };

	ID3D11ShaderResourceView* GetSRV();
	ID3D11Texture2D* GetResource() { return m_pResource; };
	void SetResource(ID3D11Texture2D* resource) { m_pResource = resource; };
	void SetResourceView(ID3D11ShaderResourceView* resourceView) { m_pResourceView = resourceView; };
	
	ColorRGB Sample(const Vector2& uv) const;

private:
	Texture() = default;
	Texture(SDL_Surface* pSurface);

	SDL_Surface* m_pSurface{ nullptr };
	uint32_t* m_pSurfacePixels{ nullptr };
	ID3D11Texture2D* m_pResource{};
	ID3D11ShaderResourceView* m_pResourceView{};
};
