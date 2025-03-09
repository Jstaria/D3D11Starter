#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>

#include "../Interfaces/ISerializable.h"

class Component : public ISerializable {
public:
    virtual ~Component() = default;
    virtual const char* GetType() const = 0;
};