#pragma once

#include "Texture.h"
#include <vector>
#include "DataTypes.h"

class MeshData
{
public:
	MeshData() = default;
	MeshData(const MeshData&) = delete;
	MeshData(MeshData&&) noexcept = delete;
	MeshData& operator=(const MeshData&) = delete;
	MeshData& operator=(MeshData&&) noexcept = delete;
	~MeshData() {
		for (auto* texture : textures)
		{
			if (texture)
			{
				delete texture;
			}
		};
	};

	std::vector<Vertex> vertices{};
	std::vector<uint32_t> indices{};
	
	Matrix worldMatrix{};
	Matrix transformMatrix{};
	Matrix scaleMatrix{};
	Matrix rotationMatrix{};
	float yawRotation{};


	/**
		* 0 => Diffuse
		* 1 => Normal
		* 2 => Specular
		* 3 => Glossiness
		*/
	std::array<std::string, 4> texturesLocations{};
	std::array<Texture*, 4> textures{};

	void AddRotationY(float yaw)
	{
		yawRotation += yaw;
		rotationMatrix = Matrix::CreateRotationY(yawRotation);
		
		// Update world matrix 
		UpdateWorldMatrix();
	};

private:
	void UpdateWorldMatrix()
	{
		worldMatrix = scaleMatrix * rotationMatrix * transformMatrix;
	}
};
