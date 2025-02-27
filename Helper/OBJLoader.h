#pragma once
#include <fstream>
#include <stdexcept>
#include "DirectXMath.h"
#include <vector>
#include "../Structures/Vertex.h"
#include "../Structures/MeshData.h"

namespace OBJLoader
{
	MeshData LoadOBJ(const char* objFile);
};

