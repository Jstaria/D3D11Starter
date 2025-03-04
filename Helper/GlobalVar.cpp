#include "GlobalVar.h"

namespace {
    float deltaTime;
    float elapsedTime;
}

// Time Var
float GlobalVar::Time::getDeltaTime() { return deltaTime; }
float GlobalVar::Time::getElapsedTime() { return elapsedTime; }
void GlobalVar::Time::setDeltaTime(float dt) { deltaTime = dt; }
void GlobalVar::Time::setElapsedTime(float time) { elapsedTime = time; }
