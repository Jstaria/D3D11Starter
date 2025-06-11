#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(DirectX::XMFLOAT3 position, std::shared_ptr<Transform> parentTransform, float particleSpeed, float emitterSpeed)
	: particleSpeed(particleSpeed), emitterSpeed(emitterSpeed)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(position);
	transform->SetParentTransform(parentTransform);
}

std::shared_ptr<IDrawable> ParticleSystem::GetDrawable() { return drawable; }
std::shared_ptr<Material> ParticleSystem::GetMaterial() { return material; }
std::shared_ptr<Transform> ParticleSystem::GetTransform() { return transform; }
std::shared_ptr<SimpleComputeShader> ParticleSystem::GetCS() { return cs; }

void ParticleSystem::Update(float deltaTime)
{
}

void ParticleSystem::Draw()
{
}


