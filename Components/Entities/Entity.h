#pragma once
#include "../GameObject.h"
#include "../Entities/Agent.h"

class Entity : public Agent
{
protected:
	std::shared_ptr<GameObject> gameObj;

public:
	Entity(std::shared_ptr<GameObject> gameObj, const Agent& agent);

	void Update(float deltaTime) override;
};

