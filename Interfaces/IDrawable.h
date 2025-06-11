#pragma once

class IDrawable {
protected:
	const char* name;

public:
	virtual void Draw() = 0;
	virtual const char* GetName() = 0;
};