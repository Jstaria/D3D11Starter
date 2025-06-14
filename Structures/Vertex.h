#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The local position of the vertex
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;

	// Kept simple for now, just the positions
	// The only reason I'd use this is for the positions anyway 
	// (I can always expand it to be more specific)
	bool operator==(const Vertex& other) const {

		return
			Position.x == other.Position.x &&
			Position.y == other.Position.y &&
			Position.z == other.Position.z;
	}
	bool operator<(const Vertex& other) const {
		if (Position.x != other.Position.x) return Position.x < other.Position.x;
		if (Position.y != other.Position.y) return Position.y < other.Position.y;
		return Position.z < other.Position.z;
	}
};