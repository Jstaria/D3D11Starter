#pragma once
#include "../Components/Component.h"

class IComponent {
public:
	virtual void AddComponent(std::shared_ptr<Component>) = 0;
	template <typename T> std::shared_ptr<T> GetComponent() const = 0;
private:
	std::unordered_map<const char*, Component> components;
};