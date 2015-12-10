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
} arm_state, ai_arm_state;
float armTimer, ai_armTimer;

float attackBuffer = 0.f; //Attack Buffer for AI

enum class LEG_STATE {
	IDLE,
	RETRACTING,
	STRETCHING,
} leg_state, ai_leg_state;
float legTimer, ai_legTimer;

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
	node->Translate(glm::vec3(0, -4, 0));
	node->Rotate(90, glm::vec3(0, 1, 0));
	m_worldNode->AddChild("body", node);

	//AI Body
	node = new SceneNode(*node);
	node->Rotate(180, glm::vec3(0, 1, 0));
	m_worldNode->AddChild("aibody", node);

	dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body"))->Translate(glm::vec3(-2, 0, -6));
	node->Translate(glm::vec3(2, 0, -6));
	
	//Arm
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("arm", "Obj//Arm.obj");
	mesh->textureID[0] = LoadTGA("Image//stick.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(-1.f, 2.5f, 0.3f));
	node->Rotate(-10, glm::vec3(0, 0, 1));
	node->Rotate(160, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->AddChild("arm", node);

	//AI Arm
	node = new SceneNode(*node);
	node->SetTransform(glm::mat4(1.f));
	node->Translate(glm::vec3(1.f, 2.5f, 0.3f));
	node->Rotate(10, glm::vec3(0, 0, 1));
	node->Rotate(160, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("aibody")->AddChild("arm", node);

	//Forearm
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("forearm", "Obj//Forearm.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0, 1.6f, 0));
	node->Rotate(80, glm::vec3(-1, 0, 0));
	node->Rotate(180.f, glm::vec3(0, 1, 0));
	m_worldNode->GetChildNode("body")->GetChildNode("arm")->AddChild("forearm", node);

	//AI Forearm
	node = new SceneNode(*node);
	m_worldNode->GetChildNode("aibody")->GetChildNode("arm")->AddChild("forearm", node);

	//Right Thigh
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("right thigh", "Obj//Right Thigh.obj");
	mesh->textureID[0] = LoadTGA("Image//stick.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(-0.4f, 0, 0.1));
	node->Rotate(-50, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->AddChild("right thigh", node);

	//AI Right Thigh
	node = new SceneNode(*node);
	m_worldNode->GetChildNode("aibody")->AddChild("right thigh", node);

	//Right Leg
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("right leg", "Obj//Right Leg.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0, -2.5, 0));
	node->Rotate(50, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->GetChildNode("right thigh")->AddChild("right leg", node);

	//AI Right Leg
	node = new SceneNode(*node);
	m_worldNode->GetChildNode("aibody")->GetChildNode("right thigh")->AddChild("right leg", node);

	//Left Thigh
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("left thigh", "Obj//Left Thigh.obj");
	mesh->textureID[0] = LoadTGA("Image//stick.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0.4f, 0, 0.1));
	node->Rotate(35, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->AddChild("left thigh", node);

	//AI Left Thigh
	node = new SceneNode(*node);
	m_worldNode->GetChildNode("aibody")->AddChild("left thigh", node);

	//Left Leg
	node = new SceneNode();
	mesh = MeshBuilder::GenerateOBJ("left leg", "Obj//Left Leg.obj");
	mesh->textureID[0] = LoadTGA("Obj//dummy_wood.tga");
	node->SetMesh(mesh);
	node->Translate(glm::vec3(0, -2.5, 0));
	node->Rotate(10, glm::vec3(1, 0, 0));
	m_worldNode->GetChildNode("body")->GetChildNode("left thigh")->AddChild("left leg", node);

	//AI Left Leg
	node = new SceneNode(*node);
	m_worldNode->GetChildNode("aibody")->GetChildNode("left thigh")->AddChild("left leg", node);

	arm_state = ARM_STATE::IDLE;
	armTimer = 0.f;

	ai_arm_state = ARM_STATE::IDLE;
	ai_armTimer = 0.f;

	leg_state = LEG_STATE::IDLE;
	legTimer = 0.f;

	ai_leg_state = LEG_STATE::IDLE;
	ai_legTimer = 0.f;
}

#define CAMERA_SPEED 20.f
void (Camera3::*cameraMovement[4])(float dist) = { &Camera3::MoveForward, &Camera3::MoveBackward, &Camera3::MoveLeft, &Camera3::MoveRight };

const float armRaisingTime = 0.5f, armRetractingTime = 0.7f;
MyModel::PLAYER_COMMANDS attackTarget, ai_attackTarget;
const std::map<MyModel::PLAYER_COMMANDS, float> armAttackAngle = {
	{ MyModel::PLAYER_COMMANDS::ATTACK_UP, -80.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_MID, -70.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_DOWN, -40.f } };
const std::map<MyModel::PLAYER_COMMANDS, float> forearmAttackAngle = {
	{ MyModel::PLAYER_COMMANDS::ATTACK_UP, -80.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_MID, -80.f },
	{ MyModel::PLAYER_COMMANDS::ATTACK_DOWN, -80.f } };

const float legRetractingTime = 0.4f, legStretchingTime = 0.4f;
MyModel::PLAYER_COMMANDS moveDirection, ai_moveDirection;
const float thighMoveAngle = -30.f;
const float legMoveAngle = 40.f;

void MyModel::Update(double dt)
{
	srand(time(NULL));

	//Controls arm movement
	playerAttackFSM(dt);

	//Controls AI arm movement
	aiAttackFSM(dt);

	//Controls leg movement
	playerMoveFSM(dt);

	//Controls Ai leg movement
	aiMoveFSM(dt);

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

void MyModel::playerAttackFSM(double dt)
{
	//Get Nodes
	SceneNode *armNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("arm"));
	SceneNode *forearmNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("arm")->GetChildNode("forearm"));

	//Initial Arm Transformation
	static const glm::mat4 c_armrotate = armNode->GetTransform();
	static const glm::mat4 c_forearmrotate = forearmNode->GetTransform();

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
}

void MyModel::aiAttackFSM(double dt)
{
	//Get Nodes
	SceneNode *ai_armNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("aibody")->GetChildNode("arm"));
	SceneNode *ai_forearmNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("aibody")->GetChildNode("arm")->GetChildNode("forearm"));

	//Initial Arm Transformation
	static const glm::mat4 c_armrotate = ai_armNode->GetTransform();
	static const glm::mat4 c_forearmrotate = ai_forearmNode->GetTransform();

	switch (ai_arm_state)
	{
	case ARM_STATE::IDLE:
		//AI arm randomizer
		if (attackBuffer > 1.0f)
		{
			if (rand() % 100 < 75)
			{
				switch (rand() % 3)
				{
				case 0:
					ai_attackTarget = PLAYER_COMMANDS::ATTACK_UP;
					break;
				case 1:
					ai_attackTarget = PLAYER_COMMANDS::ATTACK_MID;
					break;
				case 2:
					ai_attackTarget = PLAYER_COMMANDS::ATTACK_DOWN;
					break;
				}
				ai_arm_state = ARM_STATE::RAISING;
			}
			attackBuffer = 0.f;
		}
		else attackBuffer += dt;
		break;
	case ARM_STATE::RAISING:
		ai_armTimer += dt;
		if (ai_armTimer < armRaisingTime)
		{
			ai_armNode->Rotate(dt / armRaisingTime * armAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
			ai_forearmNode->Rotate(dt / armRaisingTime * forearmAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
		}
		else
		{
			ai_arm_state = ARM_STATE::RETRACTING;
			ai_armTimer = 0.f;
		}
		break;
	case ARM_STATE::RETRACTING:
		ai_armTimer += dt;
		if (ai_armTimer < armRetractingTime)
		{
			ai_armNode->Rotate(-dt / armRetractingTime * armAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
			ai_forearmNode->Rotate(-dt / armRetractingTime * forearmAttackAngle.at(attackTarget), glm::vec3(1, 0, 0));
		}
		else
		{
			ai_arm_state = ARM_STATE::IDLE;
			ai_armNode->SetTransform(c_armrotate);
			ai_forearmNode->SetTransform(c_forearmrotate);
			ai_armTimer = 0.f;
		}
		break;
	}
}

void MyModel::playerMoveFSM(double dt)
{
	//Get Nodes
	SceneNode *bodyNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body"));
	SceneNode *thighNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("left thigh"));
	SceneNode *legNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body")->GetChildNode("left thigh")->GetChildNode("left leg"));

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
}

float distancebetween,AiPosition;
bool close = false;
bool idlemovement = true;
static int chance = 33;

void MyModel::aiMoveFSM(double dt)
{
	SceneNode *AiTorsoNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("aibody"));
	SceneNode *torsoNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("body"));
	SceneNode *AibodyNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("aibody"));
	SceneNode *AithighNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("aibody")->GetChildNode("left thigh"));
	SceneNode *AilegNode = dynamic_cast<SceneNode*>(m_worldNode->GetChildNode("aibody")->GetChildNode("left thigh")->GetChildNode("left leg"));
	
	//Case
	switch (ai_leg_state)
	{
	case LEG_STATE::IDLE:
			{
				//Get the distance between the player and the AI
				distancebetween = AiTorsoNode->GetTransform()[3][0] - torsoNode->GetTransform()[3][0];
				AiPosition = AiTorsoNode->GetTransform()[3][0];
				std::cout << AiPosition << std::endl;
				//Changing of boolean
				if (distancebetween < 500.f)
				{
					close = true;
					idlemovement = false;
					if (AiPosition < 1538.f)
					{
						ai_leg_state = LEG_STATE::RETRACTING;
						ai_legTimer = 0.f;
					}
				}
				else if (distancebetween < 1000.f) // Move right
				{
					
					close = true;
					idlemovement = false;

					if (AiPosition < 1538.f)
					{
						if (rand() % 100 <= chance)
						{
							ai_leg_state = LEG_STATE::RETRACTING;
							ai_legTimer = 0.f;
						}
					}
				}
				else if (distancebetween > 1500.f) // Move left
				{
					close = false;
					idlemovement = false;

					if (rand() % 100 <= chance)
					{
						ai_leg_state = LEG_STATE::RETRACTING;
						ai_legTimer = 0.f;
					}
				}
				else
				{
					idlemovement = true;
				}
			}
		break;
	case LEG_STATE::RETRACTING:
		ai_legTimer += dt;
		if (ai_legTimer < legRetractingTime)
		{
			AithighNode->Rotate(dt / legRetractingTime * thighMoveAngle, glm::vec3(1, 0, 0));
			AilegNode->Rotate(dt / legRetractingTime * legMoveAngle, glm::vec3(1, 0, 0));
			if (close == true && idlemovement == false)
				AibodyNode->Translate(glm::vec3(0, 0, -3 * dt));
		}
		else
		{
			ai_leg_state = LEG_STATE::STRETCHING;
			ai_legTimer = 0.f;
		}
		break;
	case LEG_STATE::STRETCHING:
		ai_legTimer += dt;
		if (ai_legTimer < legStretchingTime)
		{
			AithighNode->Rotate(-dt / legRetractingTime * thighMoveAngle, glm::vec3(1, 0, 0));
			AilegNode->Rotate(-dt / legRetractingTime * legMoveAngle, glm::vec3(1, 0, 0));
			if (close == false && idlemovement == false) AibodyNode->Translate(glm::vec3(0, 0, 3 * dt));
		}
		else
		{
			ai_leg_state = LEG_STATE::IDLE;
			ai_legTimer = 0.f;
		}
		break;
	}
}	