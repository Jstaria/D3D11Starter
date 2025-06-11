#include "Entity.h"

Entity::Entity(std::shared_ptr<GameObject> gameObj, const Agent& agent)
	: Agent(agent), gameObj(gameObj)
{
}

void Entity::Update(float deltaTime)
{
	gameObj->Update(deltaTime);
	Agent::Update(deltaTime);
}
