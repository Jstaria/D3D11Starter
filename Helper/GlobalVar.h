#pragma once
namespace GlobalVar
{
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

};

