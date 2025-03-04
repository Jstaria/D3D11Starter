#pragma once
namespace GlobalVar
{
	namespace Time {
		
		// -=| Getters |=-
		float getDeltaTime();
		float getElapsedTime();

		// -=| Setters |=-
		void setDeltaTime(float dt);
		void setElapsedTime(float time);
	}

};

