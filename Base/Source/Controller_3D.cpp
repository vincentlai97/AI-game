#include "Controller_3D.h"

Controller_3D::~Controller_3D()
{
}

void Controller_3D::Init()
{
	Controller::Init();

	glfwSetInputMode(view->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Controller_3D::Update()
{
	MyModel *model = dynamic_cast<MyModel*>(this->model);

#ifdef WASD_MOVEMENT
	if (IsKeyPressed('W')) model->SetCommands((int)MyModel::COMMANDS::MOVE_FORWARD);
	if (IsKeyPressed('S')) model->SetCommands((int)MyModel::COMMANDS::MOVE_BACKWARD);
	if (IsKeyPressed('A')) model->SetCommands((int)MyModel::COMMANDS::MOVE_LEFT);
	if (IsKeyPressed('D')) model->SetCommands((int)MyModel::COMMANDS::MOVE_RIGHT);
#endif

#ifdef MOUSE_VIEW
	double mouseX, mouseY;
	glfwGetCursorPos(view->getWindow(), &mouseX, &mouseY);
	model->SetCursorPos(mouseX, mouseY);
	glfwSetCursorPos(view->getWindow(), 0, 0);
#endif

	if (IsKeyPressed('J')) model->SetCommands(MyModel::PLAYER_COMMANDS::ATTACK_UP);
	if (IsKeyPressed('K')) model->SetCommands(MyModel::PLAYER_COMMANDS::ATTACK_MID);
	if (IsKeyPressed('L')) model->SetCommands(MyModel::PLAYER_COMMANDS::ATTACK_DOWN);
	if (IsKeyPressed('Z')) model->SetCommands(MyModel::PLAYER_COMMANDS::MOVE_FORWARD);
	if (IsKeyPressed('X')) model->SetCommands(MyModel::PLAYER_COMMANDS::MOVE_BACKWARD);

	Controller::Update();
}