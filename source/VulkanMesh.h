#pragma once

#include <vector>

//#include <assimp/scene.h>
#include <vulkan/vulkan.h>

#include "Mesh.h"

struct Model
{
	Matrix model{};
};


class VulkanMesh
{
public:
	MeshData* GetMeshData();

	Matrix GetModel();
	void SetModel(Matrix newModel);

	VulkanMesh() = default;
	VulkanMesh(
		VkPhysicalDevice newPhysicalDevice,
		VkDevice newDevice,
		VkQueue transferQueue,
		VkCommandPool transferCommandPool,
		std::vector<Vertex>* vertices,
		std::vector<uint32_t>* indices,
		int newTexId,
		MeshData* meshData
	);
	~VulkanMesh();

	int GetTexId();

	uint32_t GetVertexCount();
	uint32_t GetIndexCount();
	VkBuffer GetVertexBuffer();
	VkBuffer GetIndexBuffer();

	void DestroyBuffers();

	//static std::vector<std::string> LoadMaterials(const aiScene* scene);
	//static std::vector<Mesh> LoadNode(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool,
	//	aiNode* node, const aiScene* scene, std::vector<int> mat2Tex);
	//static Mesh LoadMesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, VkQueue transferQueue, VkCommandPool transferCommandPool,
	//	aiMesh* mesh, const aiScene* scene, std::vector<int> mat2Tex);


private:
	MeshData* m_MeshData;
	Matrix m_Model;

	int m_TexId{};

	int32_t m_VertexCount{};
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	uint32_t m_IndexCount{};
	VkBuffer m_IndexBuffer{};
	VkDeviceMemory m_IndexBufferMemory{};

	VkPhysicalDevice m_PhysicalDevice{};
	VkDevice m_Device{};

	void CreateVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices);
	void CreateIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices);
};
