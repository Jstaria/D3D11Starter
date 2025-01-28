#include "Mesh.h"

Mesh::Mesh(Vertex* vertices, unsigned int* indices, unsigned int vertSize, unsigned int indexSize)
{
	CreateMesh(vertices, indices, vertSize, indexSize);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateMesh(Vertex* vertices, unsigned int* indices, unsigned int vertSize, unsigned int indexSize)
{
	// --- Create Vertex Buffer ---
	{
		// Set vertex count by finding the difference in memory from address
		vertexCount = vertSize;
		printf("Vertex Count: %d\n", vertexCount);

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(Vertex) * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData = {};
		vertexData.pSysMem = vertices;

		Graphics::Device->CreateBuffer(&vbd, &vertexData, VertexBuffer.GetAddressOf());
	}
	
	// --- Create Index Buffer ---
	{
		// Set index count by finding the difference in memory from address
		indexCount = indexSize;
		printf("Index Count: %d\n", indexCount);

		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(unsigned int) * indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = indices;

		Graphics::Device->CreateBuffer(&ibd, &indexData, IndexBuffer.GetAddressOf());
	}
}

ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return VertexBuffer;
}

ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return IndexBuffer;
}

unsigned int Mesh::GetVertexCount()
{
	return vertexCount;
}

unsigned int Mesh::GetIndexCount()
{
	return indexCount;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	Graphics::Context->IASetVertexBuffers(1, 0, VertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Graphics::Context->DrawIndexed(indexCount, 0, 0);
}
