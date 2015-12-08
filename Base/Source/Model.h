#ifndef MODEL_H
#define MODEL_H

#include "Object.h"
#include "Camera3.h"
#include "Scene Graph\SceneNode.h"

#include <vector>

class Model
{
public:
	enum class COMMANDS :int
	{
		NUM_COMMANDS,
	};

	virtual void Init();
	virtual void Update(double dt);

	virtual void SetCommands(int command) = 0;
	virtual void SetCursorPos(double cursorX, double cursorY) { cursorPos[0] = cursorX; cursorPos[1] = cursorY; }

	const std::vector<Object *>& getObjectList() const { return m_objectList; }
	const Camera3& getCamera() const { return camera; }
	SceneNode* getWorldNode() { return m_worldNode; }

protected:
	std::vector<Object *> m_objectList;

	Camera3 camera;

	SceneNode *m_worldNode;

	bool *commands;
	double cursorPos[2];

};

#endif