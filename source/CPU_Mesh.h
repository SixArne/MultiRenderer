#pragma once
#include "Mesh.h"
#include "DataTypes.h"

class CPU_Mesh
{
public:
	CPU_Mesh(MeshData* meshData, PrimitiveTopology topology);
	CPU_Mesh(const CPU_Mesh&) = delete;
	CPU_Mesh(CPU_Mesh&&) noexcept = delete;
	CPU_Mesh& operator=(const CPU_Mesh&) = delete;
	CPU_Mesh& operator=(CPU_Mesh&&) noexcept = delete;

	MeshData* GetMeshData() { return m_pMeshData;  };
	std::vector<Vertex_Out>& GetVerticesOut() { return m_pVerticesOut; };
	PrimitiveTopology GetPrimitiveTopology() { return m_PrimitiveTopology; };

private:
	MeshData* m_pMeshData{};
	std::vector<Vertex_Out> m_pVerticesOut{};
	PrimitiveTopology m_PrimitiveTopology{ PrimitiveTopology::TriangleStrip };
};

