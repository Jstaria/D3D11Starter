#pragma once

#include "Triangle.h"
#include <memory>

struct NavTile {
	Triangle tileTriangle;
	std::shared_ptr<NavTile> neighbors[3];
};