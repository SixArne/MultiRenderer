#pragma once
#include "Math.h"
#include "array"
#include <string>
#include "Texture.h"


struct Vertex
{
	Vector3 position{};
	Vector2 uv{}; //W3
	Vector3 normal{}; //W4
	Vector3 tangent{}; //W4
};

struct Vertex_Out
{
	Vector4 position{};
	Vector2 uv{};
	Vector3 normal{};
	Vector3 tangent{};
	Vector3 viewDirection{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};

