#include "Mesh.h"

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

/// <summary>
/// Initializes mesh using low level mesh data
/// </summary>
/// <param name="name"></param>
/// <param name="vertices"></param>
/// <param name="indices"></param>
/// <param name="vertSize"></param>
/// <param name="indexSize"></param>
Mesh::Mesh(const char* name, std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	MeshData meshData = {};
	meshData.indices = indices;
	meshData.name = name;
	meshData.vertices = vertices;

	InitializeMesh(meshData);
}

Mesh::Mesh(MeshData meshData)
{
	InitializeMesh(meshData);
}

Mesh::Mesh(const char* name, const char* filePath)
{
	MeshData meshData = OBJLoader::LoadOBJ(filePath);
	meshData.name = name;

	InitializeMesh(meshData);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	indexBuffer.ReleaseAndGetAddressOf();
	vertexBuffer.ReleaseAndGetAddressOf();
}

/// <summary>
/// Will always initialize common mesh components
/// </summary>
/// <param name="name"></param>
/// <param name="vertices"></param>
/// <param name="indices"></param>
/// <param name="vertSize"></param>
/// <param name="indexSize"></param>
void Mesh::InitializeMesh(MeshData meshData)
{
	// Will set name and generate buffers for the mesh
	this->name = meshData.name;
	this->meshData = meshData;
	
	CreateMesh(meshData);
	FindCenterOfMesh(meshData);

	// Make sure the default values 
	// for each mesh don't override Debug values

	meshToggle = true;
	wireFrameToggle = true;
}

void Mesh::CreateMesh(MeshData meshData)
{
	// --- Create Vertex Buffer ---
	{
		unsigned int vertexCount = (unsigned int)meshData.vertices.size();

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &meshData.vertices[0];

		Graphics::Device->CreateBuffer(&vbd, &vertexData, vertexBuffer.GetAddressOf());
	}

	// --- Create Index Buffer ---
	{
		unsigned int indexCount = (unsigned int)meshData.indices.size();

		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &meshData.indices[0];

		Graphics::Device->CreateBuffer(&ibd, &indexData, indexBuffer.GetAddressOf());
	}
}

void Mesh::FindCenterOfMesh(MeshData meshData)
{
	int triangleCount = (int)meshData.indices.size() / 3;
	Vertex* vertices = new Vertex[triangleCount]();

	if (meshData.indices.size() % 3 != 0) {
		throw std::runtime_error("Mesh indices size is not a multiple of 3.");
	}

	for (int i = 0; i < meshData.indices.size(); i += 3)
	{
		int index = i / 3;
		if (index >= triangleCount) {
			throw std::runtime_error("Index out of bounds for vertices array.");
		}

		Vertex v1 = meshData.vertices[meshData.indices[i]];
		Vertex v2 = meshData.vertices[meshData.indices[i + 1]];
		Vertex v3 = meshData.vertices[meshData.indices[i + 2]];

		Vertex c1{};
		c1.Position = XMFLOAT3(
			(v1.Position.x + v2.Position.x + v3.Position.x) / 3,
			(v1.Position.y + v2.Position.y + v3.Position.y) / 3,
			(v1.Position.z + v2.Position.z + v3.Position.z) / 3
		);

		vertices[index] = c1;
	}

	Vertex c{};
	for (int i = 0; i < triangleCount; i++)
	{
		c.Position.x += vertices[i].Position.x;
		c.Position.y += vertices[i].Position.y;
		c.Position.z += vertices[i].Position.z;
	}

	c.Position.x /= triangleCount;
	c.Position.y /= triangleCount;
	c.Position.z /= triangleCount;

	center = c;

	delete[] vertices; 
}

void Mesh::CalcTangents()
{
}

ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetVertexCount()
{
	return (int)meshData.vertices.size();
}

int Mesh::GetIndexCount()
{
	return (int)meshData.indices.size();
}

const char* Mesh::GetName()
{
	return name;
}

Vertex Mesh::GetCenter()
{
	return center;
}

bool* Mesh::GetToggleMesh()
{
	return &meshToggle;
}

bool* Mesh::GetToggleWireFrame()
{
	return &wireFrameToggle;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Will show mesh based on debug mesh toggle
	if (meshToggle) {

		// Will show mesh based on this mesh's toggle
		if (Debug::ShowMesh) {
			Graphics::Context->RSSetState(Debug::RasterizerFillState.Get());
			Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			Graphics::Context->DrawIndexed((int)meshData.vertices.size(), 0, 0);
		}
	}

	// Will show WF based on debug mesh toggle
	if (wireFrameToggle)
	{
		// Will show WF based on this WF's toggle
		if (Debug::ShowWireFrame) {
			Graphics::Context->RSSetState(Debug::RasterizerWFState.Get());
			Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			Graphics::Context->DrawIndexed((int)meshData.indices.size(), 0, 0);
		}
	}
}
