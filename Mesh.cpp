#include "Mesh.h"



void Mesh::CreateMesh()
{
}

ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return VertexBuffer;
}

ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return IndexBuffer;
}

int Mesh::GetVertexCount()
{
	return vertexCount;
}

int Mesh::GetIndexCount()
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
