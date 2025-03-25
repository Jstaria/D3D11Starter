#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "../MainComponents/Graphics.h"
#include "../Helper/Debug.h"
#include "../Helper/OBJLoader.h"
#include "../Structures/Vertex.h"
#include "../Structures/MeshData.h"

class Mesh
{
private:
	// --- Mesh Buffers ---
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	MeshData meshData;

	// --- General ---
	const char* name;
	bool meshToggle;
	bool wireFrameToggle;
	Vertex center;

	// --- Mesh Creation ---
	void InitializeMesh(MeshData meshData);
	void CreateMesh(MeshData meshData);
	//MeshData LoadData(const char* filePath);
	void FindCenterOfMesh(MeshData meshData);
	void CalcTangents();

public:
	// --- Constructors ---
	Mesh(const char* name, std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	Mesh(MeshData meshData);
	Mesh(const char* name, const char* filePath);
	Mesh();
	~Mesh();

	// --- Mesh Getters ---
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetVertexCount();
	int GetIndexCount();
	const char* GetName();

	Vertex GetCenter();

	bool* GetToggleMesh();
	bool* GetToggleWireFrame();

	// --- General ---
	void Draw();
};

