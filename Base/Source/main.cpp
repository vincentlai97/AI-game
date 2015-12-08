#include <stdio.h> //Include the standard C++ headers
#include <stdlib.h>

#include <GL/glew.h> //Include GLEW
#include <GLFW/glfw3.h> //Include GLFW
#include <glm.hpp>

glm::vec3 vector;

#include "Model.h"
#include "MyModel.h"
#include "View.h"
#include "Controller.h"
#include "Controller_3D.h"

void main( void )
{
	Model* model = new MyModel();
	View* view = new View(model);
	Controller* controller = new Controller_3D(model, view);

	// Initialize GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);
	controller->Init();
	glewExperimental = true; // Needed for core profile
	// Initialize GLEW
	if (glewInit() != GLEW_OK) exit(EXIT_FAILURE);
	view->Init();
	model->Init();

	controller->RunLoop();

	if (model != NULL) delete model;
	if (view != NULL) delete view;
	if (controller != NULL) delete controller;
}