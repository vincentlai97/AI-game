#ifndef MYMODEL_H
#define MYMODEL_H

#include "Model.h"
#include <array>
#include <map>

class MyModel : public Model
{
public:
	enum class COMMANDS : int
	{
		MOVE_FORWARD,
		MOVE_BACKWARD,
		MOVE_LEFT,
		MOVE_RIGHT,
		NUM_COMMANDS,
	};

	enum class PLAYER_COMMANDS
	{
		ATTACK_UP,
		ATTACK_MID,
		ATTACK_DOWN,
		MOVE_FORWARD,
		MOVE_BACKWARD,
	};

	MyModel();
	~MyModel();

	virtual void Init();
	virtual void Update(double dt);
	virtual void SetCommands(int command);
	void SetCommands(PLAYER_COMMANDS command) { player_commands[command] = true; }

protected:
	std::map<PLAYER_COMMANDS, bool> player_commands;

	void playerAttackFSM(double dt);
	void aiAttackFSM(double dt);
	void playerMoveFSM(double dt);
	void aiMoveFSM(double dt);
	
};

#define NUM_PLAYER_COMMANDS 5U
const std::array<MyModel::PLAYER_COMMANDS, NUM_PLAYER_COMMANDS> all_player_commands = {
	MyModel::PLAYER_COMMANDS::ATTACK_UP,
	MyModel::PLAYER_COMMANDS::ATTACK_MID,
	MyModel::PLAYER_COMMANDS::ATTACK_DOWN,
	MyModel::PLAYER_COMMANDS::MOVE_FORWARD,
	MyModel::PLAYER_COMMANDS::MOVE_BACKWARD };

#define NUM_PLAYER_COMMANDS_ATTACK 3U
const std::array<MyModel::PLAYER_COMMANDS, NUM_PLAYER_COMMANDS_ATTACK> all_player_commands_attack = {
	MyModel::PLAYER_COMMANDS::ATTACK_UP,
	MyModel::PLAYER_COMMANDS::ATTACK_MID,
	MyModel::PLAYER_COMMANDS::ATTACK_DOWN };

#define NUM_PLAYER_COMMANDS_MOVE 2U
const std::array<MyModel::PLAYER_COMMANDS, NUM_PLAYER_COMMANDS_MOVE> all_player_commands_move = {
	MyModel::PLAYER_COMMANDS::MOVE_FORWARD,
	MyModel::PLAYER_COMMANDS::MOVE_BACKWARD };

#endif