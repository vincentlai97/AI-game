#include "Controller.h"

// GLEW & GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "timer.h"
#include <map>

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

//Define the cursor enter callback
static void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (entered == GL_FALSE)
	{
		glfwSetCursorPos(window, 400, 300);
	}
}

//Define the window resize callback
static void resize_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void Controller::Init()
{
	//Set error callback
	glfwSetErrorCallback(error_callback);

	//Initialise GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	view->createWindow(1280, 720, "Scene");

	m_dElapsedTime = 0.f;
}

const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
static double m_dAccumulatedTime_ThreadOne = 0.0;
static double m_dAccumulatedTime_ThreadTwo = 0.0;
StopWatch m_timer;

void Controller::RunLoop()
{
	m_timer.startTimer();
	while (!glfwWindowShouldClose(view->getWindow()) && !IsKeyPressed(GLFW_KEY_ESCAPE))
	{
		// Get the elapsed time
		m_dElapsedTime = m_timer.getElapsedTime();
		m_dAccumulatedTime_ThreadOne += m_dElapsedTime;
		m_dAccumulatedTime_ThreadTwo += m_dElapsedTime;
		Update();
		view->Render();
		//Swap buffers
		glfwSwapBuffers(view->getWindow());
		glfwPollEvents();
		m_timer.waitUntil(frameTime); // Frame rate limiter. Limits each frame to a specified time in ms.   
	}
}

void Controller::Update()
{
	model->Update(m_dElapsedTime);
}