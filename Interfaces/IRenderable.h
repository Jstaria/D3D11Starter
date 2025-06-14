#pragma once

#include <memory>

#include "../Components/Mesh.h"
#include "../Components/Transform.h"
#include "../Components/Material.h"

class IRenderable {

protected:

	std::shared_ptr<Material> material;
	std::shared_ptr<IDrawable> drawable;
	std::shared_ptr<Transform> transform;

public:

	virtual std::shared_ptr<IDrawable> GetDrawable() = 0;
	virtual std::shared_ptr<Material> GetMaterial() = 0;
	virtual std::shared_ptr<Transform> GetTransform() = 0;

	virtual void SetMaterial(std::shared_ptr<Material> material) = 0;
	virtual void SetDrawable(std::shared_ptr<IDrawable> drawable) = 0;
	virtual void Draw() = 0;
};