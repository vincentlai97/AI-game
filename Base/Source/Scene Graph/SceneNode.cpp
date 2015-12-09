#include "SceneNode.h"


SceneNode::SceneNode()
: mesh(nullptr)
, transform(glm::mat4(100.f))
, scale(1, 1, 1)
, Node()
{
}


SceneNode::~SceneNode()
{
}
