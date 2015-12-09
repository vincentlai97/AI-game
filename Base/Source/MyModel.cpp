#include "MyModel.h"

#include "Win32Input.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"

#define ARM_DEBUG

MyModel::MyModel()
{
	commands = new bool[(int)COMMANDS::NUM_COMMANDS] {false};

	for (auto iter : all_player_commands)
	{
		player_commands[iter] = false;
	}

	m_worldNode = new SceneNode();
}

MyModel::~MyModel()
{
}

Mesh *mesh;
Object *object;
SceneNode *node;

enum class ARM_STATE {
	IDLE,
	RAISING,
	RETRACTING,
} arm_state;
float armTimer;

float attackBuffer = 0.f; //Attack Buffer for AI

enum class LEG_STATE {
	IDLE,
	RETRACTING,
	STRETCHING,
} leg_state;
float legTimer;

void MyModel::Init()
{
	camera.Init(Vector3(0, 5, 5), Vector3(), Vector3(0, 1, 0));

	m_worldNode->SetMesh(MeshBuilder::GenerateQuad("background", Color(0, 0, 0.5f), 1.f));
	m_worldNode->GetMesh()->textureID[0] = LoadTGA("Image//background.tga");
	m_worldNode->Scale(glm::vec3(32, 18, 1));

	//Body
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("body", "Obj//Mannequin Body.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(-6, -4, 2));
	node->Rotate(90, glm::vec3(0, 1, 0));
	m_worldNode->AddChild("body", node);
	
	//Arm
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("arm", "Obj//Arm.obj");
	mesh->textureID[0] = LoadTGA("Image//stick.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(-1.f, 2.5f, 0.3f));
	node->Rotate(10, glm::vec3(0, 0, 1));
	node->Rotate(160, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->AddChild("arm", node);

	//Forearm
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("forearm", "Obj//Forearm.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0, 1.6f, 0));
	node->Rotate(80, glm::vec3(-1, 0, 0));
	node->Rotate(180.f, glm::vec3(0, 1, 0));
	m_worldNode->GetChildNode("body")->GetChildNode("arm")->AddChild("forearm", node);

	//Right Thigh
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("right thigh", "Obj//Right Thigh.obj");
	mesh->textureID[0] = LoadTGA("Image//stick.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(-0.4f, 0, 0.1));
	node->Rotate(-50, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->AddChild("right thigh", node);

	//Right Leg
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("right leg", "Obj//Right Leg.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0, -2.5, 0));
	node->Rotate(50, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->GetChildNode("right thigh")->AddChild("right leg", node);

	//Left Thigh
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("left thigh", "Obj//Left Thigh.obj");
	mesh->textureID[0] = LoadTGA("Image//stick.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0.4f, 0, 0.1));
	node->Rotate(35, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->AddChild("left thigh", node);

	//Left Leg
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("left leg", "Obj//Left Leg.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0, -2.5, 0));
	node->Rotate(10, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->GetChildNode("left thigh")->AddChild("left leg", node);

	arm_state = ARM_STATE::IDLE;
	armTimer = 0.f;

	leg_state = LEG_STATE::IDLE;
	legTimer = 0.f;
}

#define CAMERA_SPEED 20.f
void (Camera3::*cameraMovement[4])(float dist) = { &Camera3::MoveForward, &Camera3::MoveBackward, &Camera3::MoveLeft, &Camera3::MoveRight };

const float armRaisingTime = 0.5f, armRetractingTime = 0.7f;
MyModel::PLAYER_COMMANDS attackTarget;
const std::map<MyModel::PLAYER_COMMANDS, float> armAttackAngle = {
	{ MyModel::PLAYER_COMMANDS::ATTACK_UP, -80.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_MID, -70.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_DOWN, -40.f } };
const std::map<MyModel::PLAYER_COMMANDS, float> forearmAttackAngle = {
	{ MyModel::PLAYER_COMMANDS::ATTACK_UP, -80.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_MID, -80.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_DOWN, -80.f } };

const float legRetractingTime = 0.4f, legStretchingTime = 0.4f;
MyModel::PLAYER_COMMANDS moveDirection;
const float thighMoveAngle = -30.f;
const float legMoveAngle = 40.f;

void MyModel::Update(double dt)
{
	srand(time(NULL));

	//Get Nodes
	SceneNode *bodyNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body"));
	SceneNode *armNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("arm"));
	SceneNode *forearmNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("arm")->GetChildNode("forearm"));
	SceneNode *thighNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("left thigh"));
	SceneNode *legNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("left thigh")->GetChildNode("left leg"));

	//Initial Arm Transformation
	static const glm::mat4 c_armrotate = armNode->GetTransform();
	static const glm::mat4 c_forearmrotate = forearmNode->GetTransform();

	//AI randomizer
	/*if (attackBuffer > 1.0f && arm_state == ARM_STATE::IDLE)
	{
		if (rand() % 100 < 75)
		{
			attackTarget = (rand() % 3);
			arm_state = ARM_STATE::RAISING;
		}
		attackBuffer = 0.f;
	}
	else if (arm_state == ARM_STATE::IDLE) attackBuffer += dt;*/

	//Controls arm movement
	switch (arm_state)
	{
	case ARM_STATE::IDLE:
#ifdef ARM_DEBUG
		std::cout << "Idle" << std::endl;
#endif
		for (auto iter : all_player_commands_attack)
		{
			if (player_commands[iter])
			{
				arm_state = ARM_STATE::RAISING;
				attackTarget = iter;
				armTimer = 0.f;
				break;
			}
		}
		break;
	case ARM_STATE::RAISING:
		armTimer += dt;
		if (armTimer < armRaisingTime)
		{
			armNode->Rotate(dt / armRaisingTime * armAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
			forearmNode->Rotate(dt / armRaisingTime * forearmAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
		}
		else
		{
			arm_state = ARM_STATE::RETRACTING;
			armTimer = 0.f;
		}
#ifdef ARM_DEBUG
		switch (attackTarget)
		{
		case PLAYER_COMMANDS::ATTACK_UP:
			std::cout << "Attack Up" << std::endl;
			break;
		case PLAYER_COMMANDS::ATTACK_MID:
			std::cout << "Attack Mid" << std::endl;
			break;
		case PLAYER_COMMANDS::ATTACK_DOWN:
			std::cout << "Attack Down" << std::endl;
			break;
		}
#endif
		break;
	case ARM_STATE::RETRACTING:
		armTimer += dt;
		if (armTimer < armRetractingTime)
		{
			armNode->Rotate(-dt / armRetractingTime * armAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
			forearmNode->Rotate(-dt / armRetractingTime * forearmAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
		}
		else
		{
			arm_state = ARM_STATE::IDLE;
			armNode->SetTransform(c_armrotate);
			forearmNode->SetTransform(c_forearmrotate);
			armTimer = 0.f;
		}
#ifdef ARM_DEBUG
		std::cout << "Retracting" << std::endl;
#endif
		break;
	}

	switch (leg_state)
	{
	case LEG_STATE::IDLE:
		for (auto iter : all_player_commands_move)
		{
			if (player_commands[iter])
			{
				moveDirection = iter;
				leg_state = LEG_STATE::RETRACTING;
				legTimer = 0.f;
				break;
			}
		}
		break;
	case LEG_STATE::RETRACTING:
		legTimer += dt;
		if (legTimer < legRetractingTime)
		{
			thighNode->Rotate(dt / legRetractingTime * thighMoveAngle, glm::vec3(1, 0, 0));
			legNode->Rotate(dt / legRetractingTime * legMoveAngle, glm::vec3(1, 0, 0));
			if (moveDirection == PLAYER_COMMANDS::MOVE_BACKWARD) bodyNode->Translate(glm::vec3(0, 0, -3 * dt));
		}
		else
		{
			leg_state = LEG_STATE::STRETCHING;
			legTimer = 0.f;
		}
		break;
	case LEG_STATE::STRETCHING:
		legTimer += dt;
		if (legTimer < legStretchingTime)
		{
			thighNode->Rotate(-dt / legRetractingTime * thighMoveAngle, glm::vec3(1, 0, 0));
			legNode->Rotate(-dt / legRetractingTime * legMoveAngle, glm::vec3(1, 0, 0));
			if (moveDirection == PLAYER_COMMANDS::MOVE_FORWARD) bodyNode->Translate(glm::vec3(0, 0, 3 * dt));
		}
		else
		{
			leg_state = LEG_STATE::IDLE;
			legTimer = 0.f;
		}
		break;
	}

	for (int iter = (int)COMMANDS::MOVE_FORWARD; iter <= (int)COMMANDS::MOVE_RIGHT; ++iter)
	if (commands[iter])
		(camera.*(cameraMovement[iter - (int)COMMANDS::MOVE_FORWARD]))(dt * CAMERA_SPEED);

	camera.LookUp(cursorPos[1] * 0.03f);
	camera.LookLeft(cursorPos[0] * 0.03f);

	std::fill(commands, &commands[(int)COMMANDS::NUM_COMMANDS], false);
	for (auto iter : all_player_commands) player_commands[iter] = false;
}

void MyModel::SetCommands(int command)
{
	commands[command] = true;
}