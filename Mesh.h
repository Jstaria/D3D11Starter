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
	unsigned int vertexCount;
	unsigned int indexCount;

	// --- Mesh Creation ---
	void CreateMesh(Vertex vertices[], unsigned int indices[], unsigned int vertSize, unsigned int indexSize);
	void LoadData(char filePath[]);

public:
	// --- Constructor ---
	Mesh(Vertex vertices[], unsigned int indices[], unsigned int vertSize, unsigned int indexSize);
	Mesh(char* filePath);
	Mesh();
	~Mesh();

	// --- Mesh Getters ---
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetVertexCount();
	unsigned int GetIndexCount();
	void Draw();
};

