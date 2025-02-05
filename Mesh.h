#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include "Graphics.h"
#include "Debug.h"
#include "MeshData.h"
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
	bool meshToggle;
	bool wireFrameToggle;
	Vertex center;

	// --- Mesh Creation ---
	void InitializeMesh(MeshData meshData);
	void CreateMesh(MeshData meshData);
	MeshData LoadData(char filePath[]);
	void FindCenterOfMesh(MeshData meshData);

public:
	// --- Constructors ---
	Mesh(const char* name, Vertex* vertices, unsigned int* indices, int vertSize, int indexSize);
	Mesh(MeshData meshData);
	Mesh(char* filePath);
	Mesh();
	~Mesh();

	// --- Mesh Getters ---
	ComPtr<ID3D11Buffer> GetVertexBuffer();
	ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetVertexCount();
	int GetIndexCount();
	const char* GetName();

	Vertex GetCenter();

	bool* GetToggleMesh();
	bool* GetToggleWireFrame();

	// --- General ---
	void Draw();
};

