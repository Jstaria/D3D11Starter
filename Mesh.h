#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include "Graphics.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

class Mesh
{
private:
	// --- Mesh Buffers ---
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;

	// --- Mesh Counts ---
	int vertexCount;
	int indexCount;

	void CreateMesh();

public:
	// --- Constructor ---
	Mesh(/*Data to be read in*/);
	~Mesh();

	// --- Mesh Getters ---
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetVertexCount();
	int GetIndexCount();
	void Draw();
};

