#include "NavMesh.h"

using namespace std;
using namespace DirectX;

void NavMesh::GenerateTiles(MeshData meshData, float walkableDegree)
{
	DirectX::XMFLOAT3 tileNormal{ 0,1,0 };

	transform->SetPosition(tileNormal.x / 5, tileNormal.y / 5, tileNormal.z / 5);

	vector<unsigned int> indices;
	vector<Vertex> vertices;

	for (int i = 0; i < meshData.indices.size() / 3; i++)
	{
		Vertex v1 = meshData.vertices[meshData.indices[i * 3 + 0]];
		Vertex v2 = meshData.vertices[meshData.indices[i * 3 + 1]];
		Vertex v3 = meshData.vertices[meshData.indices[i * 3 + 2]];

		Triangle tri{
			Edge{v1,v2},
			Edge{v2,v3},
			Edge{v3,v1},
			v1,v2,v3,
			v1.Normal
		};

		DirectX::XMFLOAT3 normal = tri.normal;

		XMVECTOR ogNormalVec = XMLoadFloat3(&tileNormal);
		XMVECTOR normalVec = XMLoadFloat3(&normal);

		float dot = XMVectorGetX(XMVector3Dot(ogNormalVec, normalVec));
		float angle = acos(dot) / Pi;

		if (angle < walkableDegree) {
			navMeshTiles.push_back(make_shared<NavTile>());
			navMeshTiles[navMeshTiles.size() - 1]->tileTriangle = tri;

			int i0 = static_cast<int>(navMeshTiles.size() - 1) * 3 + 0;
			int i1 = static_cast<int>(navMeshTiles.size() - 1) * 3 + 1;
			int i2 = static_cast<int>(navMeshTiles.size() - 1) * 3 + 2;

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);

			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);
		}
	}

	//for (int i = 0; i < navMeshTiles.size(); i++)
	//{
	//	shared_ptr<NavTile> t1 = navMeshTiles[i];

	//	for (int j = 0; j < navMeshTiles.size(); j++)
	//	{
	//		if (i == j) continue;
	//	}
	//}

	meshData = {};

	meshData.indices = indices;
	meshData.vertices = vertices;

	drawable = make_shared<Mesh>(meshData);
}

void NavMesh::GenerateMesh()
{

}

int NavMesh::AddOrGetVertex(Vertex v, std::map<Vertex, int>& vertexMap, vector<Vertex>& vertices)
{
	auto found = vertexMap.find(v);
	if (found == vertexMap.end()) {
		int newIndex = static_cast<int>(vertices.size());
		vertices.push_back(v);

		vertexMap[v] = newIndex;

		return newIndex;
	}

	return found->second;
}

std::shared_ptr<NavTile> NavMesh::FindNeighboringTile(const NavTile& tile)
{
	return std::shared_ptr<NavTile>();
}

NavMesh::NavMesh(MeshData meshData, std::shared_ptr<Material> material, std::shared_ptr<Transform> transform, float walkableDegree)
{
	this->transform = std::make_shared<Transform>();
	this->transform->SetParentTransform(transform);

	transform->SetChildTransform(this->transform.get());

	this->material = material;

	GenerateTiles(meshData, walkableDegree);
	//GenerateMesh();
}

void NavMesh::Draw()
{
	if (!Debug::ShowNavMesh) return;

	drawable->Draw();
}

std::shared_ptr<Transform> NavMesh::GetTransform() { return transform; }
std::shared_ptr<IDrawable> NavMesh::GetDrawable() { return drawable; }
std::shared_ptr<Material> NavMesh::GetMaterial() { return material; }

void NavMesh::SetMaterial(std::shared_ptr<Material> material) { this->material = material; }
void NavMesh::SetDrawable(std::shared_ptr<IDrawable> drawable) { this->drawable = drawable; }
