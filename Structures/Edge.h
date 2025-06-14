#pragma once

#include <DirectXMath.h>
#include "Vertex.h"

struct Edge {
	Vertex v1;
	Vertex v2;

	bool operator==(const Edge& other) const {
		return (v1 == other.v1 && v2 == other.v2) ||
			(v1 == other.v2 && v2 == other.v1);
	}
};