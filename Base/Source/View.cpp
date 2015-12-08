#include "View.h"

#include <gtc\matrix_transform.hpp>
#include <gtc\matrix_inverse.hpp>
#include <gtx\transform.hpp>

#include "shader.hpp"
#include "MeshBuilder.h"
#include "LoadTGA.h"

#include <sstream>

#include "MyModel.h"

#define VIEW_DIMENSIONS 3

bool View::createWindow(const int& window_width, const int& window_height, const char* window_title, GLFWmonitor* monitor, GLFWwindow* share)
{
	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	m_window = glfwCreateWindow(window_width, window_height, window_title, monitor, share);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	return true;
}

Mesh* gBufferQuad;
unsigned gBufferTexture[GBuffer::GBUFFER_NUM_TEXTURES];

#define UNIFORM_TYPE (unsigned)UNIFORM_TYPE
#define GEOMETRY_PASS_UNIFORM_TYPE (unsigned)GEOMETRY_PASS_UNIFORM_TYPE
#define LIGHT_PASS_UNIFORM_TYPE (unsigned)LIGHT_PASS_UNIFORM_TYPE

void View::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	int window_width, window_height;
	glfwGetWindowSize(m_window, &window_width, &window_height);
	m_gBuffer.Init(window_width, window_height);

	m_geometryPassShaderID = LoadShaders("Shader//GeometryPass.vertexshader", "Shader//GeometryPass.fragmentshader");

	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MVP] = glGetUniformLocation(m_geometryPassShaderID, "MVP");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MODELVIEW] = glGetUniformLocation(m_geometryPassShaderID, "MV");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_geometryPassShaderID, "MV_inverse_transpose");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_LIGHTENABLED] = glGetUniformLocation(m_geometryPassShaderID, "lightEnabled");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_AMBIENT] = glGetUniformLocation(m_geometryPassShaderID, "material.kAmbient");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_geometryPassShaderID, "material.kDiffuse");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_SPECULAR] = glGetUniformLocation(m_geometryPassShaderID, "material.kSpecular");
	m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_SHININESS] = glGetUniformLocation(m_geometryPassShaderID, "material.kShininess");
	for (int count = 0; count < MAX_TEXTURES; ++count)
	{
		m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_TEXTURE_ENABLED + count] = glGetUniformLocation(m_geometryPassShaderID, std::string("textureEnabled[" + std::to_string(long double(count)) + "]").c_str());
		m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_TEXTURE + count] = glGetUniformLocation(m_geometryPassShaderID, std::string("texture[" + std::to_string(long double(count)) + "]").c_str());
	}

	m_lightPassShaderID = LoadShaders("Shader//LightPass.vertexshader", "Shader//LightPass2.fragmentshader");

	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_MVP] = glGetUniformLocation(m_lightPassShaderID, "MVP");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_VIEW] = glGetUniformLocation(m_lightPassShaderID, "V");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_SCREENSIZE] = glGetUniformLocation(m_lightPassShaderID, "screenSize");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_POSITION_MAP] = glGetUniformLocation(m_lightPassShaderID, "positionMap");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_NORMAL_MAP] = glGetUniformLocation(m_lightPassShaderID, "normalMap");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_AMBIENT_MAP] = glGetUniformLocation(m_lightPassShaderID, "ambientMap");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_DIFFUSE_MAP] = glGetUniformLocation(m_lightPassShaderID, "diffuseMap");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_SPECULAR_MAP] = glGetUniformLocation(m_lightPassShaderID, "specularMap");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHTENABLED] = glGetUniformLocation(m_lightPassShaderID, "lightEnabled");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_TYPE] = glGetUniformLocation(m_lightPassShaderID, "light.type");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_POSITION] = glGetUniformLocation(m_lightPassShaderID, "light.position");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_COLOR] = glGetUniformLocation(m_lightPassShaderID, "light.color");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_POWER] = glGetUniformLocation(m_lightPassShaderID, "light.power");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_KC] = glGetUniformLocation(m_lightPassShaderID, "light.kC");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_KL] = glGetUniformLocation(m_lightPassShaderID, "light.kL");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_KQ] = glGetUniformLocation(m_lightPassShaderID, "light.kQ");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_DIRECTION] = glGetUniformLocation(m_lightPassShaderID, "light.direction");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_lightPassShaderID, "light.cosCutoff");
	m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_COSINNER] = glGetUniformLocation(m_lightPassShaderID, "light.cosInner");

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 0, 1);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 10;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].direction.Set(-1.f, -1.f, -1.f);

	lights[1] = lights[0];

	gBufferQuad = MeshBuilder::GenerateQuad("GBufferQuad", Color(1.f, 1.f, 1.f), 1.f);
	for (int count = 0; count < GBuffer::GBUFFER_NUM_TEXTURES; ++count)
	{
		gBufferTexture[count] = m_gBuffer.GetTexture((GBuffer::GBUFFER_TEXTURE_TYPE)count);
	}
}

#define camera m_model->getCamera()

void View::Render()
{
#if VIEW_DIMENSIONS == 2
	int windowWidth, windowHeight;
	glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
	projectionStack.push(glm::ortho(-(float)windowWidth / 20, (float)windowWidth / 20, -(float)windowHeight / 20, (float)windowHeight / 20, -1000.f, 1000.f));
	viewStack.push(glm::mat4(1.f));
#else if VIEW_DIMENSIONS == 3
	projectionStack.push(glm::perspective(45.f, 4.f / 3.f, .1f, 10000.f));
	viewStack.push(glm::lookAt(
		glm::vec3(camera.position.x, camera.position.y, camera.position.z),
		glm::vec3(camera.target.x, camera.target.y, camera.target.z),
		glm::vec3(camera.up.x, camera.up.y, camera.up.z)
		));
#endif
	modelStack.push(glm::mat4(1.f));

	m_renderPass = RENDER_PASS::RENDER_PASS_GEOMETRY;
	m_gBuffer.BindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_geometryPassShaderID);

	std::vector<Object *> objectList = m_model->getObjectList();
	for (std::vector<Object *>::iterator it = objectList.begin(); it != objectList.end(); ++it)
	{
		modelStack.push(modelStack.top()); {
			modelStack.top() *= (*it)->translation;
			modelStack.top() *= (*it)->rotation;
			modelStack.top() *= (*it)->scale;

			RenderMesh((*it)->mesh, false);
		} modelStack.pop();
	}

	RenderWorldSceneNode(m_model->getWorldNode());

	m_renderPass = RENDER_PASS::RENDER_PASS_LIGHT;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int window_width, window_height;
	glfwGetWindowSize(m_window, &window_width, &window_height);
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_lightPassShaderID);
	glDisable(GL_CULL_FACE);

	glUniform1i(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_POSITION], 1, &lights[0].position.x);
	glUniform3fv(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_KQ], lights[0].kQ);
	glUniform3fv(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_DIRECTION], 1, &lights[0].direction.x);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHT0_EXPONENT], lights[0].exponent);

	for (unsigned count = 0; count < GBuffer::GBUFFER_NUM_TEXTURES; ++count)
	{
		m_gBuffer.BindForReading(GL_TEXTURE0 + count, count);
		glUniform1i(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_POSITION_MAP + count], count);
	}

	float screenSize[2] = { window_width, window_height };
	glUniform2fv(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_SCREENSIZE], 1, &screenSize[0]);

	Mesh *RenderQuad = MeshBuilder::GenerateQuad("lightPass", Color(), 2.f);
	projectionStack.push(glm::ortho(-1.f, 1.f, -1.f, 1.f));
	viewStack.push(glm::mat4(1));
	RenderMesh(RenderQuad, false);
	viewStack.pop();
	projectionStack.pop();

	glEnable(GL_CULL_FACE);

	projectionStack.pop();
	viewStack.pop();
	modelStack.pop();
}

void View::RenderMesh(Mesh *mesh, bool lightEnabled, unsigned offset, unsigned count)
{
	glm::mat4 MVP = projectionStack.top() * viewStack.top() * modelStack.top();

	if (m_renderPass == RENDER_PASS::RENDER_PASS_GEOMETRY)
	{
		glUniformMatrix4fv(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP[0][0]);

		glUniform1i(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_LIGHTENABLED], lightEnabled);
		if (lightEnabled)
		{
			glm::mat4 modelView = viewStack.top() * modelStack.top();
			glUniformMatrix4fv(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MODELVIEW], 1, GL_FALSE, &modelView[0][0]);
			glm::mat4 modelView_inverse_transpose = glm::inverseTranspose(modelView);
			glUniformMatrix4fv(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView_inverse_transpose[0][0]);

			//load material
			glUniform3fv(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
			glUniform3fv(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
			glUniform3fv(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
			glUniform1f(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_MATERIAL_SHININESS], mesh->material.kShininess);
		}
		for (int count = 0; count < MAX_TEXTURES; ++count)
		{
			if (mesh->textureID[count])
			{
				glUniform1i(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_TEXTURE_ENABLED + count], 1);
				glActiveTexture(GL_TEXTURE0 + count);
				glBindTexture(GL_TEXTURE_2D, mesh->textureID[count]);
				glUniform1i(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_TEXTURE + count], count);
			}
			else
			{
				glUniform1i(m_geometryPassParameters[GEOMETRY_PASS_UNIFORM_TYPE::U_TEXTURE_ENABLED + count], 0);
			}
		}
	}
	else if (m_renderPass == RENDER_PASS::RENDER_PASS_LIGHT)
	{
		glUniformMatrix4fv(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_MVP], 1, GL_FALSE, &MVP[0][0]);
		if (lightEnabled)
		{
			glUniform1i(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_LIGHTENABLED], 1);
			glUniform4fv(m_lightPassParameters[LIGHT_PASS_UNIFORM_TYPE::U_VIEW], 1, &viewStack.top()[0][0]);
		}
		else
		{
			glUniform1i(m_lightPassParameters[UNIFORM_TYPE::U_LIGHTENABLED], 0);
		}
	}

	mesh->Render(offset, count);
}

void View::Render2DMesh(Mesh *mesh, bool lightEnabled, unsigned offset, unsigned count)
{
	int windowWidth, windowHeight;
	glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
	projectionStack.push(projectionStack.top());
	projectionStack.top() = glm::ortho(0.f, (float)windowWidth, 0.f, (float)windowHeight, -10.f, 10.f);
	viewStack.push(glm::mat4(1.f));

	RenderMesh(mesh, lightEnabled, offset, count);

	projectionStack.pop();
	viewStack.pop();
}

void View::RenderWorldSceneNode(SceneNode *node)
{
	modelStack.push(modelStack.top()); {
		modelStack.top() *= node->GetTransform();
		if (node->GetMesh() != nullptr)
			RenderMesh(node->GetMesh(), false);
		for (auto iter : node->GetChildNodes())
		{
			RenderWorldSceneNode(dynamic_cast<SceneNode*>(node->GetChildNode(iter.first)));
		}
	} modelStack.pop();
}

/*void View::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	int windowWidth, windowHeight;
	glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
	Mtx44 ortho;
	ortho.SetToOrtho(0, windowWidth, 0, windowHeight, -10, 10);
	projectionStack.push(glm::ortho(0.f, (float)windowWidth, 0.f, (float)windowHeight, -10.f, 10.f));
	viewStack.push(glm::mat4(1.f));
	modelStack.push(glm::mat4(1.f));
	modelStack.top() *= glm::translate((float)windowWidth, (float)windowHeight, 0.f);
	modelStack.top() *= glm::scale(size, size, size);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID[0]);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		glm::mat4 characterSpacing = glm::translate(i * 1.f + .5f, .5f, 0.f);
		glm::mat4 MVP = projectionStack.top() * viewStack.top() * modelStack.top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP[0][0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	modelStack.pop();
	viewStack.pop();
	projectionStack.pop();
	glEnable(GL_DEPTH_TEST);
}*/

#undef camera