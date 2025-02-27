#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "Vertex.h"
#include <vector>

struct MeshData
{
	const char* name;

	// -=| Vector |=-
	std::vector<DirectX::XMFLOAT3> positions;	// Positions from the file
	std::vector<DirectX::XMFLOAT3> normals;		// Normals from the file
	std::vector<DirectX::XMFLOAT2> uvs;		// UVs from the file
	std::vector<Vertex> vertices;		// Verts we're assembling
	std::vector<unsigned int> indices;		// Indices of these verts
};

