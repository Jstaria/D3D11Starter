#pragma once
#include <random>
#include <DirectXMath.h>
#include <d3d11.h>

namespace GlobalVar
{
	namespace Lights {

	}

	namespace Material {
		unsigned int getMaterialIndex();
		void TickMaterialIndex();
		unsigned int getIndexThenTick();
	}
	namespace Time {
		
		// -=| Getters |=-
		float getDeltaTime();
		float getElapsedTime();

		// -=| Setters |=-
		void setDeltaTime(float dt);
		void setElapsedTime(float time);
	}
	namespace Random {
		float Get(float f1, float f2);
	}
	namespace Math {
		DirectX::XMFLOAT3 Float3Slerp(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3 b, float t);
	}
};

