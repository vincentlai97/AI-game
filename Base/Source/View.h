#ifndef VIEW_H
#define VIEW_H

#include "Model.h"

//Include GLEW & GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include "Light.h"
#include "GBuffer.h"

#include <stack>

class View
{
	enum RENDER_PASS
	{
		RENDER_PASS_GEOMETRY,
		RENDER_PASS_LIGHT,
		foo,
	};

	enum class GEOMETRY_PASS_UNIFORM_TYPE : unsigned int
	{
		U_MVP,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_LIGHTENABLED,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_TEXTURE_ENABLED,
		U_TEXTURE_ENABLED_END = U_TEXTURE_ENABLED + MAX_TEXTURES - 1,
		U_TEXTURE,
		U_TEXTURE_END = U_TEXTURE + MAX_TEXTURES - 1,
		U_TOTAL
	};

	enum class LIGHT_PASS_UNIFORM_TYPE : unsigned int
	{
		U_MVP,
		U_VIEW,
		U_SCREENSIZE,
		U_POSITION_MAP,
		U_NORMAL_MAP,
		U_AMBIENT_MAP,
		U_DIFFUSE_MAP,
		U_SPECULAR_MAP,
		U_LIGHTENABLED,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_DIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_TOTAL
	};

	enum class UNIFORM_TYPE : unsigned int
	{
		U_MVP = 0,
		U_VIEW,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_TEXTURE_ENABLED,
		U_TEXTURE_ENABLED_END = U_TEXTURE_ENABLED + MAX_TEXTURES - 1,
		U_TEXTURE,
		U_TEXTURE_END = U_TEXTURE + MAX_TEXTURES - 1,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_DIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_TOTAL
	};

public:
	View(Model* model) : m_model(model) {}

	bool createWindow(const int &window_width, const int &window_height, const char *window_title, GLFWmonitor *monitor = NULL, GLFWwindow *share = NULL);
	GLFWwindow* getWindow() { return m_window; }

	virtual void Init();
	virtual void Render();

protected:
	Model *m_model;

	GLFWwindow *m_window;

	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[(unsigned)UNIFORM_TYPE::U_TOTAL];
	unsigned m_geometryPassShaderID;
	unsigned m_geometryPassParameters[(unsigned)GEOMETRY_PASS_UNIFORM_TYPE::U_TOTAL];
	unsigned m_lightPassShaderID;
	unsigned m_lightPassParameters[(unsigned)LIGHT_PASS_UNIFORM_TYPE::U_TOTAL];

	GBuffer m_gBuffer;

	RENDER_PASS m_renderPass;

	Light lights[2];

	//MS modelStack, viewStack, projectionStack;
	std::stack<glm::mat4> modelStack, viewStack, projectionStack;

	void RenderMesh(Mesh *mesh, bool lightEnabled, unsigned offset = 0, unsigned count = 0);
	void Render2DMesh(Mesh *mesh, bool lightEnabled, unsigned offset = 0, unsigned count = 0);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);

	void RenderWorldSceneNode(SceneNode *node = nullptr);
};

#endif