#include "Mesh.h"
/// <summary>
/// Initializes mesh using low level mesh data
/// </summary>
/// <param name="name"></param>
/// <param name="vertices"></param>
/// <param name="indices"></param>
/// <param name="vertSize"></param>
/// <param name="indexSize"></param>
Mesh::Mesh(const char* name, Vertex* vertices, unsigned int* indices, int vertSize, int indexSize)
{
	MeshData meshData = {};
	meshData.indexSize = indexSize;
	meshData.indices = indices;
	meshData.name = name;
	meshData.vertices = vertices;
	meshData.vertSize = vertSize;

	InitializeMesh(meshData);
}

Mesh::Mesh(MeshData meshData)
{
	InitializeMesh(meshData);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
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
	CreateMesh(meshData);
	
	// Make sure the default values 
	// for each mesh don't override Debug values

	meshToggle = true;
	wireFrameToggle = true;
}

void Mesh::CreateMesh(MeshData meshData)
{
	// --- Create Vertex Buffer ---
	{
		// Set vertex count by finding the difference in memory from address
		vertexCount = meshData.vertSize;

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
		// Set index count by finding the difference in memory from address
		indexCount = meshData.indexSize;

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
	return vertexCount;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

const char* Mesh::GetName()
{
	return name;
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

			Graphics::Context->DrawIndexed(indexCount, 0, 0);
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

			Graphics::Context->DrawIndexed(indexCount, 0, 0);
		}
	}
}
