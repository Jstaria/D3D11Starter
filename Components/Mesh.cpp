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
void Mesh::InitializeMesh(MeshData meshD)
{
	// Will set name and generate buffers for the mesh
	name = meshD.name;
	meshData = meshD;

	CalculateTangents(meshData);
	CreateMesh(meshData);
	FindCenterOfMesh(meshData);

	// Make sure the default values 
	// for each mesh don't override Debug values

	meshToggle = true;
	wireFrameToggle = true;
}

void Mesh::CreateMesh(MeshData& meshData)
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

void Mesh::FindCenterOfMesh(MeshData& meshData)
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

// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Calculates the tangents of the vertices in a mesh
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Code originally adapted from: http://www.terathon.com/code/tangent.html
//   - Updated version now found here: http://foundationsofgameenginedev.com/FGED2-sample.pdf
//   - See listing 7.4 in section 7.5 (page 9 of the PDF)
//
// - Note: For this code to work, your Vertex format must
//         contain an XMFLOAT3 called Tangent
//
// - Be sure to call this BEFORE creating your D3D vertex/index buffers
// --------------------------------------------------------
void Mesh::CalculateTangents(MeshData& meshData)
{
	// Reset tangents
	for (int i = 0; i < meshData.vertices.size(); i++)
	{
		meshData.vertices[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < meshData.indices.size();)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = meshData.indices[i++];
		unsigned int i2 = meshData.indices[i++];
		unsigned int i3 = meshData.indices[i++];
		Vertex* v1 = &meshData.vertices[i1];
		Vertex* v2 = &meshData.vertices[i2];
		Vertex* v3 = &meshData.vertices[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < meshData.vertices.size(); i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&meshData.vertices[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&meshData.vertices[i].Tangent);

		// Use Gram-Schmidt orthonormalize to ensure
		// the normal and tangent are exactly 90 degrees apart
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&meshData.vertices[i].Tangent, tangent);
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

	//// Will show mesh based on debug mesh toggle
	//if (meshToggle) {

	//	// Will show mesh based on this mesh's toggle
	//	if (Debug::ShowMesh) {
	//		Graphics::Context->RSSetState(Debug::RasterizerFillState.Get());

			Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			Graphics::Context->DrawIndexed((int)meshData.vertices.size(), 0, 0);
	//	}
	//}

	//// Will show WF based on debug mesh toggle
	//if (wireFrameToggle)
	//{
	//	// Will show WF based on this WF's toggle
	//	if (Debug::ShowWireFrame) {
	//		Graphics::Context->RSSetState(Debug::RasterizerWFState.Get());
	//		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	//		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//		Graphics::Context->DrawIndexed((int)meshData.indices.size(), 0, 0);
	//	}
	//}

	
}
