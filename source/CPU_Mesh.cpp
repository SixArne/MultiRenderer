#include "pch.h"
#include "CPU_Mesh.h"

CPU_Mesh::CPU_Mesh(MeshData* meshData, PrimitiveTopology topology)
	:m_pMeshData{ meshData }, m_PrimitiveTopology{topology}
{
}
