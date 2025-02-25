#pragma once

#include <memory>

#include "../Helper/SimpleShader.h"

class ICompute {
protected:
	std::shared_ptr<SimpleComputeShader> cs;

public:
	virtual std::shared_ptr<SimpleComputeShader> GetCS() = 0;
};