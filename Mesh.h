#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include "Graphics.h"
#include "Debug.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

class Mesh
{
private:
	// --- Mesh Buffers ---
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;

	// --- Mesh Counts ---
	unsigned int vertexCount;
	unsigned int indexCount;

	// --- General ---
	const char* name;

	// --- Mesh Creation ---
	void CreateMesh(Vertex* vertices, unsigned int* indices, int vertSize, int indexSize);
	void LoadData(char filePath[]);

public:
	// --- Constructor ---
	Mesh(const char* name, Vertex* vertices, unsigned int* indices, int vertSize, int indexSize);
	Mesh(char* filePath);
	Mesh();
	~Mesh();

	// --- Mesh Getters ---
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetVertexCount();
	int GetIndexCount();
	const char* GetName();

	// --- General ---
	void Draw();
};

