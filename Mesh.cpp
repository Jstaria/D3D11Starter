#include "Mesh.h"

Mesh::Mesh(const char* name, Vertex* vertices, unsigned int* indices, int vertSize, int indexSize)
{
	this->name = name;
	CreateMesh(vertices, indices, vertSize, indexSize);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateMesh(Vertex* vertices, unsigned int* indices, int vertSize, int indexSize)
{
	// --- Create Vertex Buffer ---
	{
		// Set vertex count by finding the difference in memory from address
		vertexCount = vertSize;

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = &vertices[0];

		Graphics::Device->CreateBuffer(&vbd, &vertexData, vertexBuffer.GetAddressOf());
	}
	
	// --- Create Index Buffer ---
	{
		// Set index count by finding the difference in memory from address
		indexCount = indexSize;

		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = &indices[0];

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

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	if (Debug::ShowMesh) {
		Graphics::Context->RSSetState(Debug::RasterizerFillState.Get());
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		Graphics::Context->DrawIndexed(indexCount, 0, 0);
	}

	if (Debug::ShowWireFrame)
	{
		Graphics::Context->RSSetState(Debug::RasterizerWFState.Get());
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		Graphics::Context->DrawIndexed(indexCount, 0, 0);
	}
}
