#pragma once
#include <string>

class ISerializable {
public:
	virtual std::string Serialize() const = 0;
	virtual void Deserialize(const std::string& data) = 0;
};