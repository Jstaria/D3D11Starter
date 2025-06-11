#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

struct BillBoardVertex {
	DirectX::XMFLOAT3 Position;	    // The local position of the billboard's center
	DirectX::XMFLOAT2 UV;
	uint32_t unitID; // Uint 0-3
};