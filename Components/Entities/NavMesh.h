#pragma once
#include <DirectXMath.h>
#include "../../Helper/Debug.h"
#include "../../Interfaces/IRenderable.h"
#include "../../Components/Mesh.h"
#include "../../Structures/MeshData.h"
#include "../../Structures/Vertex.h"
#include "../../Structures/Edge.h"
#include "../../Structures/Triangle.h"
#include "../../Structures/NavTile.h"
#include <vector>
#include <memory>
#include <map>

class NavMesh : public IRenderable
{
private:
	MeshData meshData;

	std::vector<std::shared_ptr<NavTile>> navMeshTiles;

	void GenerateTiles(MeshData meshData, float walkableDegree);
	void GenerateMesh();
	int AddOrGetVertex(Vertex v, std::map<Vertex, int>& vertexMap, std::vector<Vertex>& vertices);
	std::shared_ptr<NavTile> FindNeighboringTile(const NavTile& tile);

public:
	NavMesh(MeshData meshData, std::shared_ptr<Material> material, std::shared_ptr<Transform> transform, float walkableDegree);
	void Draw();

	// Inherited via IRenderable
	std::shared_ptr<IDrawable> GetDrawable() override;
	std::shared_ptr<Material> GetMaterial() override;
	std::shared_ptr<Transform> GetTransform() override;
	void SetMaterial(std::shared_ptr<Material> material) override;
	void SetDrawable(std::shared_ptr<IDrawable> drawable) override;
};

