#pragma once

#include "Controller.h"

#include "MyModel.h"

class Controller_3D : public Controller
{
public:
	Controller_3D(Model* model, View* view) : Controller(model, view) {}
	~Controller_3D();

	virtual void Init();

protected:
	virtual void Update();

};

#define WASD_MOVEMENT
#define MOUSE_VIEW