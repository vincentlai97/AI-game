#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Model.h"
#include "View.h"

#include <windows.h>

class Controller 
{
public:
	Controller(Model* model, View* view) : model(model), view(view) {}

	virtual void Init();
	virtual void RunLoop();

protected:
	Model* model;
	View* view;

	double m_dElapsedTime; // delta time

	virtual void Update();

	bool IsKeyPressed(int key) { return glfwGetKey(view->getWindow(), key) == GLFW_PRESS; }

};

#endif