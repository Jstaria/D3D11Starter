#pragma once

#include <memory>

#include "../Components/Mesh.h"
#include "../Components/Transform.h"
#include "../Components/Material.h"

class IRenderable {
public:
	enum RenderType {
		Object,
		ParticleSys
	};

protected:
	RenderType rendererType;
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;

public:
	virtual RenderType GetRenderType() = 0;
	virtual std::shared_ptr<Mesh> GetMesh() = 0;
	virtual std::shared_ptr<Material> GetMaterial() = 0;
	virtual std::shared_ptr<Transform> GetTransform() = 0;
	virtual void Draw() = 0;
};