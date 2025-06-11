#include "BillBoard.h"

BillBoard::BillBoard(const char* name, Microsoft::WRL::ComPtr<ID3D11InputLayout> layout)
	: layout(layout)
{
	this->name = name;

	CreateBillboard();
}

void BillBoard::Draw()
{
	UINT stride = sizeof(BillBoardVertex);
	UINT offset = 0;

	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Graphics::Context->DrawIndexed(6, 0, 0);
}

const char* BillBoard::GetName() { return name; }

void BillBoard::CreateBillboard()
{
	BillBoardVertex vertices[4];
	unsigned int indices[] = { 0,1,2,2,1,3 };

	for (int i = 0; i < 4; i++)
	{
		vertices[i].Position = { 0,0,0 };
		vertices[i].unitID = i;
		vertices[i].UV = {
			(i % 2) ? 1.0f : 0.0f,       // U: 0 or 1
			(i / 2) ? 1.0f : 0.0f        // V: 0 or 1
		};
	}
	// --- Create Vertex Buffer ---
	{
		unsigned int vertexCount = (unsigned int)4;

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof(BillBoardVertex) * 4;
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
		unsigned int indexCount = (unsigned int)6;

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