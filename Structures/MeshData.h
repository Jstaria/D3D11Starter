#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "Vertex.h"

struct MeshData
{
	// -=| Basic Mesh Data |=-
	const char* name;
	Vertex* vertices;
	unsigned int* indices;
	int vertSize;
	int indexSize;

	// -=| Other |=-
};

