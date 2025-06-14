#pragma once

class IUpdatable {
protected:
	
public:
	virtual void Update(float deltaTime) = 0;
	virtual void FixedUpdate(float deltaTime) = 0;
};