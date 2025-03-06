#include "GlobalVar.h"

namespace {
    float deltaTime;
    float elapsedTime;

    unsigned int materialIndex;
}

// Time Methods
float GlobalVar::Time::getDeltaTime() { return deltaTime; }
float GlobalVar::Time::getElapsedTime() { return elapsedTime; }
void GlobalVar::Time::setDeltaTime(float dt) { deltaTime = dt; }
void GlobalVar::Time::setElapsedTime(float time) { elapsedTime = time; }

// Material Methods
unsigned int GlobalVar::Material::getMaterialIndex() { return materialIndex; }
void GlobalVar::Material::TickMaterialIndex() { materialIndex++; }
unsigned int GlobalVar::Material::getIndexThenTick() { return materialIndex++; }

