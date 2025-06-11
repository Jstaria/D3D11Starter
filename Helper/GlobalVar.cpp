#include "GlobalVar.h"

namespace {
    float deltaTime;
    float elapsedTime;

    unsigned int materialIndex;
}

using namespace DirectX;
using namespace std;

// Time Methods
float GlobalVar::Time::getDeltaTime() { return deltaTime; }
float GlobalVar::Time::getElapsedTime() { return elapsedTime; }
void GlobalVar::Time::setDeltaTime(float dt) { deltaTime = dt; }
void GlobalVar::Time::setElapsedTime(float time) { elapsedTime = time; }

// Material Methods
unsigned int GlobalVar::Material::getMaterialIndex() { return materialIndex; }
void GlobalVar::Material::TickMaterialIndex() { materialIndex++; }
unsigned int GlobalVar::Material::getIndexThenTick() { return materialIndex++; }

float GlobalVar::Random::Get(float f1, float f2)
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(f1, f2);
	return dis(e);
}

DirectX::XMFLOAT3 GlobalVar::Math::Float3Slerp(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3 b, float t)
{
	XMVECTOR vA = XMLoadFloat3(&a);
	XMVECTOR vB = XMLoadFloat3(&b);

	vA = XMVector4Normalize(vA);
	vB = XMVector4Normalize(vB);

	float dot = XMVectorGetX(XMVector3Dot(vA, vB));
	dot = clamp(dot, -1.0f, 1.0f);

	float theta = acosf(dot) * t;

	XMVECTOR relativeVel = XMVector3Normalize(vB - dot * vA);

	XMVECTOR result = cosf(theta) * vA + sinf(theta) * relativeVel;

	XMFLOAT3 resultFloat3;
	XMStoreFloat3(&resultFloat3, result);
	return resultFloat3;
}
