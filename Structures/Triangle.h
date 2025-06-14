#pragma once

#include "Edge.h"

struct Triangle {
	Edge edges[3];
	Vertex vertices[3];
	DirectX::XMFLOAT3 normal;

	bool IsNeighbor(Triangle other) {
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				if (edges[i] == other.edges[j])
					return true;
			}
		}

		return false;
	}
};