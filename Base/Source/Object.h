#ifndef OBJECT_H
#define OBJECT_H

#include "Mesh.h"
#include <glm.hpp>

struct Object
{
	Mesh* mesh;
	glm::mat4 translation, rotation, scale;

	Object(Mesh *mesh = NULL) : mesh(mesh)
	{
		translation = glm::mat4(1.f);
		rotation = glm::mat4(1.f);
		scale = glm::mat4(1.f);
	}
};

#endif