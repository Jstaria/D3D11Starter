#pragma once

#include <DirectXMath.h>

#include "../Structures/Particle.h"
#include "../Interfaces/IRenderable.h"
#include "../Interfaces/ICompute.h"
#include "../Interfaces/IUpdatable.h"

class ParticleSystem : public IRenderable, public ICompute, public IUpdatable
{
private:
	float particleSpeed;
	float emitterSpeed;


public:
	ParticleSystem(DirectX::XMFLOAT3 position, std::shared_ptr<Transform> parentTransform, float particleSpeed, float emitterSpeed);

	std::shared_ptr<IDrawable> GetDrawable() override;
	std::shared_ptr<Material> GetMaterial() override;
	std::shared_ptr<Transform> GetTransform() override;

	// Inherited via IRenderable
	void Draw() override;

	// Inherited via ICompute
	std::shared_ptr<SimpleComputeShader> GetCS() override;

	// Inherited via IUpdatable
	void Update(float deltaTime) override;
};

